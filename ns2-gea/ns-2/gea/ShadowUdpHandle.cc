#include <apps/udp.h>

#include <gea/ShadowUdpHandle.h>
#include <gea/ShadowEventHandler.h>
#include <gea/ShadowHandle.h>
#include <gea/API.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* used for delivering parameters to new created ShadowUdpHandles */
gea::UdpHandle      *gea::ShadowUdpHandle::currentUdpHandle;
gea::UdpHandle::Mode gea::ShadowUdpHandle::currentMode;
gea::UdpAddress      gea::ShadowUdpHandle::currentUdpAddress(0,"0"); 

gea::ShadowUdpAddress::ShadowUdpAddress(int port, const char *_ip) :
    port(port),
    ip(0)
{
    in_addr tmp;
    
    if (inet_aton(_ip, &tmp)) {
	this->ip = ntohl(tmp.s_addr);
    } else {
	this->ip = ::IP_BROADCAST;
    }

    if (!strcmp(_ip, gea::UdpAddress::IP_BROADCAST))
	this->ip = ::IP_BROADCAST;
}


gea::ShadowUdpAddress::ShadowUdpAddress(const gea::ShadowUdpAddress& a) :
    port(a.port),
    ip(a.ip)
{
    
}



/* make gea::ShadowUdpHandle a Tcl aware */
static class GeaShadowUdpHandle : public TclClass {
public:
    GeaShadowUdpHandle() : TclClass("Gea/ShadowUdpHandle") {}
    

    
    TclObject* create(int argc, const char*const* argv) {
	return (new class gea::ShadowUdpHandle(gea::ShadowUdpHandle::currentMode,
					       gea::ShadowUdpHandle::currentUdpAddress));
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
    src(0,"0")
{
    

}

void gea::ShadowUdpHandle::init() {
       Tcl& tcl = Tcl::instance();
    tcl.evalf("new Agent/UDP");
    this->agent=dynamic_cast<Agent*>(TclObject::lookup(tcl.result()));
  
    if ( mode == gea::UdpHandle::Write ) { // set the destination address
	tcl.evalf("%s set dst_addr_ %d", this->agent->name(), addr.shadow->ip );
	tcl.evalf("%s set dst_port_ %d", this->agent->name(), addr.shadow->port );
    }
    
    tcl.evalf("%s attach-agent %s", this->name(), this->agent->name() );
   
 
    if (mode == gea::UdpHandle::Write) {
	tcl.evalf("%s attach %s",
		  GEA.shadow->currentNode->name(),
		  this->agent->name());
    } else { /* mode == Read */ 
	tcl.evalf("%s attach %s %d",
		  GEA.shadow->currentNode->name(),
		  this->agent->name(),
		  addr.shadow->port );
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

int gea::ShadowUdpHandle::setSrc(gea::UdpAddress src_addr) {
    return 0;
}

int gea::ShadowUdpHandle::setDest(gea::UdpAddress dest_addr) {
    this->addr = dest_addr;
    // Tcl& tcl = Tcl::instance();
    //     if ( mode == gea::UdpHandle::Write ) { // set the destination address
    // 	tcl.evalf("%s set dst_addr_ %d", this->agent->name(), addr.shadow->ip );
    // 	tcl.evalf("%s set dst_port_ %d", this->agent->name(), addr.shadow->port );
    //     }
    agent->daddr() = dest_addr.getIP();
    agent->dport() = dest_addr.getPort();
    return 0;
}

int gea::ShadowUdpHandle::write(const char *buf, int size) {
    
    if (this->handle->status != gea::Handle::Ready)
	return -1;

    this->send(size, buf);
    return 0;
}

gea::UdpAddress gea::ShadowUdpHandle::getSrc() const {

    return src;
}
	
void gea::ShadowUdpHandle::process_data(int size, ::AppData* data) { 
    
    // save the source information
    ::UdpAgent *udp = dynamic_cast<UdpAgent *>(this->agent);
    int ip = Address::instance().get_nodeaddr(udp->iph->saddr());
    {
	char buf_ip[10];
	::snprintf(buf_ip, 10, "%d", ip);
	this->src = gea::UdpAddress( udp->iph->sport(), buf_ip );
	//#warning please determine the port!
    }

    //    cout << "got some data" << endl;
    
    switch (this->handle->status) {
    case gea::Handle::Error: /* still in error state!
				That's NOT GOOD
			     */
	break;
    case gea::Handle::Blocked :
	/* good state 
	   We received some data while waiting for.
	*/
	{
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
	    
	    double t = Scheduler::instance().clock();

	    GEA.shadow->currentNode = this->handle->shadowHandle->node;
	    e(this->handle, gea::AbsTime::t0() + gea::Duration(t) ,data);
	    GEA.shadow->doPendingEvents();
	}
	break;
    case gea::Handle::Timeout:
	/* good state
	   we received some data but the timeout already occured before. 
	   
	   The only problem: What to do with the packet?
	   (1) throw it away
	   (2) buffer it for later reading.
	*/
	break;
    case gea::Handle::Ready:
	/* In ready state while receiving data 
	   Someone left us alone.
	   We previously received data and nobody cared for it.
	   That's also not good.
	*/
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
    this->timeout_timer.resched(static_cast<double>( timeout - AbsTime::now() ) );
    
}

void gea::ShadowUdpHandle::do_timeout() {
    /* store event so we can reset it ..*/

    assert( this->e);
    gea::EventHandler::Event event = this->e;

    void *data = this->data;
    
    /* reset the event */
    this->e = 0; this->data = 0;
    this->handle->status = gea::Handle::Timeout;
    
    GEA.shadow->currentNode = this->handle->shadowHandle->node;  
    
    event(this->handle, this->timeout, data);
  
    GEA.shadow->doPendingEvents();
}




/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
