/* -*-	Mode:C++; c-basic-offset:4; tab-width:8; indent-tabs-mode:t -*- */

#include <gea/Handle.h>
#include <gea/Time.h>
#include <gea/API.h>
#include <gea/Blocker.h>
#include <gea/UdpHandle.h>



using namespace std;

static const gea::AbsTime t0(gea::AbsTime::now());

static const int PORT = 4407;

class HidStRecv {
public:
    /* ::Node *node; */
    gea::Blocker blocker;
    gea::UdpHandle *udpRecv;

    int pkcount1;
    int pkmax1;
    int pkcount2;
    int pkmax2;

    HidStRecv(int argc , const char*const* argv);
    static void recv_event(gea::Handle *h, gea::AbsTime t, void *data);
};

HidStRecv::HidStRecv(int argc , const char*const* argv):
	pkcount1(0), pkmax1(0), pkcount2(0), pkmax2(0)
{
    this->udpRecv = new gea::UdpHandle( gea::UdpHandle::Read,
					gea::UdpAddress( gea::UdpAddress::IPADDR_ANY,
							 PORT  ));

    GEA.waitFor(this->udpRecv,
		gea::AbsTime::now() + gea::Duration(1.),
		HidStRecv::recv_event,
		this);
}


void HidStRecv::recv_event(gea::Handle *h, gea::AbsTime t, void *data) {
    HidStRecv *self = reinterpret_cast<class HidStRecv *>(data);

    char buf[2000];

    if (h->status == gea::UdpHandle::Ready) {
	    h->read(buf, 2000);
	    GEA.dbg() << " recv_event: " << *((int*)buf) << endl;

	    if ((*((int*)buf) % 2) == 0) {
		    self->pkcount2++;
		    if (*((int*)buf)/2 > self->pkmax2) self->pkmax2 = *((int*)buf)/2;} else {
			    self->pkcount1++;
			    if (*((int*)buf)/2 > self->pkmax1) self->pkmax1 = *((int*)buf)/2;  }

// *((int*)buf)/2 : packet-id
// counter pkcount


    }//if h->status
    else {
	    GEA.dbg() << " timeout " << endl;
    }



    GEA.waitFor( h, t + gea::Duration(2.),
		 HidStRecv::recv_event,
		 self);

if (self->pkmax1 != 0) {


   float perc1 = (float) 100*(self->pkmax1 - self->pkcount1) / (self->pkmax1);
// loss rate percentage: 100*(packetmax - packetcount)/packetmax


    cout << "1 - Recv: " << self->pkcount1 << " Lost: " << self->pkmax1 - self->pkcount1 << " Loss Rate: " << perc1 << "%" << endl;
}//if pkmax1!=0

if (self->pkmax2 != 0) {

   float perc2 = (float) 100*(self->pkmax2 - self->pkcount2) / (self->pkmax2);

    cout << "2 - Recv: " << self->pkcount2 << " Lost: " << self->pkmax2 - self->pkcount2 << " Loss Rate: " << perc2 << "%" << endl;
}//if pkmax2!=0


}


int gea_main(int argc, const char * const *argv) {

    new HidStRecv(argc,argv);

    return 0;
}
