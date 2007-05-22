/* -*-	Mode:C++; c-basic-offset:4; tab-width:8; indent-tabs-mode:t -*- */

#include <gea/Handle.h>
#include <gea/Time.h>
#include <gea/API.h>
#include <gea/Blocker.h>
#include <gea/UdpHandle.h>

#include <iostream>

using namespace std;

static const gea::AbsTime t0(gea::AbsTime::now());

static const int PORT = 4407;

class Blink {
public:
    /* ::Node *node; */
    gea::Blocker blocker;
    gea::UdpHandle *udp;
    gea::UdpHandle *udpRecv;
    
    int nr;
    
    Blink(int argc , const char*const* argv);
    static void blink_event(gea::Handle *h, gea::AbsTime t, void *data);
    static void write_ready_event(gea::Handle *h, gea::AbsTime t, void *data);
    static void recv_event(gea::Handle *h, gea::AbsTime t, void *data);
};



Blink::Blink(int argc , const char*const* argv) :
    nr(10)
{

    
    this->udp = new gea::UdpHandle( gea::UdpHandle::Write,
				    gea::UdpAddress(gea::UdpAddress::IPADDR_BROADCAST, 
						    PORT)
				    );
    
    this->udpRecv = new gea::UdpHandle(gea::UdpHandle::Read,
				       gea::UdpAddress(gea::UdpAddress::IPADDR_ANY, 
						       PORT)
				       );

     GEA.waitFor(this->udpRecv, 
 		gea::AbsTime::now() + gea::Duration(1.) ,
 		Blink::recv_event,
 		this);
    

    GEA.waitFor(&blocker, 
		gea::AbsTime::now() + 0.01,
		Blink::blink_event,
		this);
}


void Blink::blink_event(gea::Handle *h, gea::AbsTime t, void *data) {
    
    
     
    Blink *self = reinterpret_cast<class Blink *>(data);

    if (self->nr > 0)
    GEA.waitFor( &( self->blocker ), 
		 t + gea::Duration(1),
		 Blink::blink_event,
		 data);
    
    GEA.waitFor( self->udp, 
		 t + gea::Duration(0.09),
 		 Blink::write_ready_event,
 		 data);
    
    
    GEA.dbg() << "blink " << self->nr-- << " at " << ( t - t0 ) << endl;
    
}

void Blink::write_ready_event(gea::Handle *h, gea::AbsTime t, void *data) {
  
    //  cout << ( t - gea::AbsTime::t0()) << ": trying to send  " <<  endl;
    
    Blink *self = reinterpret_cast<class Blink *>(data);    
    
    const char buf[1024] = "Hallo Welt";
    if ( self->udp->write(buf,1000) == -1)
	GEA.dbg() << "cannot send" << endl;;
}
    


void Blink::recv_event(gea::Handle *h, gea::AbsTime t, void *data) {
    
    Blink *self = reinterpret_cast<Blink *>(data);
    
    char buf[2000];
    if (h->status == gea::UdpHandle::Ready) {
	h->read(buf, 2000);
	GEA.dbg() << " recv_event:  "<< buf << endl;
    
    } else  {
	
	GEA.dbg() << " timeout " << endl;
	
    }

    if (self->nr > 0)    
    GEA.waitFor(h, t + gea::Duration(2.),
		Blink::recv_event,
		self);
    
    
}



extern "C"
int gea_main(int argc, const char * const *argv) {
    
    new Blink(argc,argv);
    
    return 0;
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
