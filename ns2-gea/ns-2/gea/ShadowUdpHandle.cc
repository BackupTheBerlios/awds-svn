#include <apps/udp.h>

#include "ShadowUdpHandle.h"
#include "ShadowEventHandler.h"
#include "ShadowHandle.h"
#include <gea/API.h>
#include <common/node.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

TclObject *shadow_currentNode;

/* 
   how is the udp data tranfered between the GEA UDP Handles?

   ShadowUdpHandle ShadowUdpHandle
       |	      ^   
       V              | 
   GeaUdpApp      GeaUdpApp
       |	      ^
       V	      |
   UdpAgent       UdpAgent
       |	      ^    
       V	      |    
     Node          Node     
       |              ^  
       V              |
     [       ns2        ]
     
     I know it's insane. If you find a better way way, feel free to replace it.
*/


/* used for delivering parameters to new created ShadowUdpHandles */
gea::UdpHandle      *gea::ShadowUdpHandle::currentUdpHandle;
gea::UdpHandle::Mode gea::ShadowUdpHandle::currentMode;
gea::UdpAddress      gea::ShadowUdpHandle::currentUdpAddress(0,0); 

// gea::ShadowUdpAddress::ShadowUdpAddress(int port, const char *_ip) :
//     port(port),
//     ip(0)
// {
//     in_addr tmp;
    
//     if (inet_aton(_ip, &tmp)) {
// 	this->ip = ntohl(tmp.s_addr);
//     } else {
// 	this->ip = IP_BROADCAST;
//     }

//     if (!strcmp(_ip, gea::UdpAddress::IP_BROADCAST))
// 	this->ip = IP_BROADCAST;
// }


// gea::ShadowUdpAddress::ShadowUdpAddress(const gea::ShadowUdpAddress& a) :
//     port(a.port),
//     ip(a.ip)
// {
    
// }



/* make gea::ShadowUdpHandle a Tcl aware */
static class GeaShadowUdpHandle : public TclClass {
public:
    GeaShadowUdpHandle() : TclClass("Gea/ShadowUdpHandle") {}
    

    
    TclObject* create(int argc, const char*const* argv) {
	return ( new gea::ShadowUdpHandle(gea::ShadowUdpHandle::currentMode,
					       gea::ShadowUdpHandle::currentUdpAddress) );
    }
} class_gea_shadow_udp_app;


int gea::ShadowUdpHandle::send(int size, const char *data) {
    
    ::PacketData *pd = new PacketData(size);
    assert(pd != 0);
    memcpy(reinterpret_cast<char *>(pd->data()),
	   data, size);
    this->agent->sendmsg(size, pd);
    
    return 0;
}


gea::ShadowUdpHandle::ShadowUdpHandle(gea::UdpHandle::Mode mode, 
				      const gea::UdpAddress& addr) :
    
    timeout_timer(this),
    handle(ShadowUdpHandle::currentUdpHandle),
    e(0),
    data(0),
    timeout(gea::AbsTime::now()),
    mode(mode),
    addr(addr),
    src(0,0)
{
    

}

void gea::ShadowUdpHandle::init() {
       Tcl& tcl = Tcl::instance();
    tcl.evalf("new Agent/UDP");
    this->agent=dynamic_cast<Agent*>(TclObject::lookup(tcl.result()));
  
    if ( mode == gea::UdpHandle::Write ) { // set the destination address
	tcl.evalf("%s set dst_addr_ %d", this->agent->name(), (int)(int32_t)addr.ip );
	tcl.evalf("%s set dst_port_ %d", this->agent->name(), (int)(int32_t)addr.port );
    }
    
    tcl.evalf("%s attach-agent %s", this->name(), this->agent->name() );
   
    ShadowEventHandler * shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler); 
    if (mode == gea::UdpHandle::Write) {
	

	
	tcl.evalf("%s attach %s",
		  shadow->getCurrentNode()->name(),
		  this->agent->name());
    } else { /* mode == Read */ 
	tcl.evalf("%s attach %s %d",
		  shadow->getCurrentNode()->name(),
		  this->agent->name(),
		  (int)addr.port );
    }
    
}


gea::ShadowUdpHandle::~ShadowUdpHandle() {
    
}

int gea::ShadowUdpHandle::read (char *buf, int size) {
    
    if (this->handle->status != gea::Handle::Ready)
	return -1;
    
    assert  (this->curData);
    int dsize = size;
    if ( this->curData->size() < dsize)
	dsize = curData->size();
    memcpy(buf, this->curData->data(), dsize );
    return dsize;
}

int gea::ShadowUdpHandle::setSrc(const gea::UdpAddress& src_addr) {
    return 0;
}

void gea::ShadowUdpHandle::setDest(const gea::UdpAddress& dest_addr) {
    this->addr = dest_addr;
    agent->daddr() = dest_addr.getIP();
    agent->dport() = dest_addr.getPort();
}

int gea::ShadowUdpHandle::write(const char *buf, int size) {
    //    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (this->handle->status != gea::Handle::Ready)
	return -1;

    this->send(size, buf);
    return size;
}

gea::UdpAddress gea::ShadowUdpHandle::getSrc() const {

    return src;
}

gea::UdpAddress gea::ShadowUdpHandle::getDest() const {
    return addr;
}
	
void gea::ShadowUdpHandle::process_data(int size, ::AppData* data) { 

    // save the source information
    ::UdpAgent *udp = dynamic_cast<UdpAgent *>(this->agent);
    int ip = Address::instance().get_nodeaddr(udp->iph->saddr());
    {
	this->src = gea::UdpAddress( ip, udp->iph->sport() );
    }

    switch (this->handle->status) {
    case gea::Handle::Error: /* still in error state!
				That's NOT GOOD
			     */
	    std::cout << "error" << std::endl;
	break;
    case gea::Handle::Blocked :
	/* good state 
	   We received some data while waiting for it.
	*/
	{
	    //	    std::cout << "blocked" << std::endl;
	    /*cancel the timeout */
	    this->timeout_timer.cancel();
	    
	    this->handle->status = gea::Handle::Ready;
	    this->curData = dynamic_cast<PacketData *>(data);
	    
	    assert (this->e && "we need an event handler");
	    /* store event so we can reset it ..*/
	    gea::EventHandler::Event e = this->e;
	    void *data = this->data;
	    
	    /* reset the event */
	    this->e = 0; this->data = 0;
	    
	    //double t = Scheduler::instance().clock();
	    
	    ShadowEventHandler *shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler);
	    
	    shadow->setCurrentNode(this->handle->shadowHandle->node);
	    GEA.lastEventTime = gea::AbsTime::now();
	    e(this->handle, GEA.lastEventTime ,data);
	    shadow->doPendingEvents(GEA.lastEventTime);
	}
	break;
    case gea::Handle::Timeout:
	/* good state
	   we received some data but the timeout already occured before. 
	   
	   The only problem: What to do with the packet?
	   (1) throw it away
	   (2) buffer it for later reading.
	*/
	//	    std::cout << "timeout" << std::endl;
	break;
    case gea::Handle::Ready:
	/* In ready state while receiving data 
	   Someone left us alone.
	   We previously received data and nobody cared for it.
	   That's also not good.
	*/
	//	    std::cout << "ready" << std::endl;
	break;
    case gea::Handle::Undefined: /* fall through ! */
    default:
	assert(!"insane handle state");
    } // end switch(this->status) 
    
} // end process_data


void gea::ShadowUdpHandle::setup_event(gea::EventHandler::Event e,
				       void *data,
				       AbsTime timeout) {
    
    this->e = e;
    this->data = data;
    this->timeout = timeout;
    this->timeout_timer.resched(( timeout - AbsTime::now() ).getSecondsD() );
    
}

void gea::ShadowUdpHandle::do_timeout() {
    /* store event so we can reset it ..*/

    assert( this->e);
    gea::EventHandler::Event event = this->e;

    void *data = this->data;
    
    /* reset the event */
    this->e = 0; this->data = 0;
    this->handle->status = gea::Handle::Timeout;
    
    ShadowEventHandler *shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler);
    
    shadow->setCurrentNode(this->handle->shadowHandle->node);  
    GEA.lastEventTime = this->timeout;
    event(this->handle, GEA.lastEventTime, data);
    
    shadow->doPendingEvents(GEA.lastEventTime);
}




/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
