#include <gea/Handle.h>
#include <gea/Time.h>
#include <gea/API.h>
#include <gea/Blocker.h>
#include <gea/DependHandle.h>



static const double PERIOD = .5;



using namespace std;
using namespace gea;


struct FreqDiv : public DependHandle {

    const unsigned div; 
    unsigned cur;
    FreqDiv(unsigned div=3) : div(div), cur(0) {}
    
    void beep() {
	if (++cur == div) {
	    cur = 0;
	    this->complied();
	}
    }

};



class DepTest {
    
public:
    Blocker      blocker;
    DependHandle dep1, dep2;
    FreqDiv      freqDiv;
    
    DepTest();

    static void blocker_ready(Handle *h, AbsTime t, void *data);
    static void dep1_ready(Handle *h, AbsTime t, void *data);
    static void dep2_ready(Handle *h, AbsTime t, void *data);
    static void freq_ready(Handle *h, AbsTime t, void *data);
};


void DepTest::blocker_ready(Handle *h, AbsTime t, void *data) {

    DepTest *self = static_cast<DepTest *>(data);
    
    static int blocker_nr = 0;
    GEA.dbg() << "blocker ready " << ++blocker_nr << endl; 
    
    // reschedule ...
    GEA.waitFor(h, t + Duration(PERIOD), DepTest::blocker_ready, data);
    
    self->freqDiv.beep();
}

void DepTest::freq_ready(Handle *h, AbsTime t, void *data) {
    if (h->status == Handle::Timeout)
	GEA.dbg() << "freq Timeout" << endl;
    else {
	GEA.dbg() << "freq okay" << endl;
	static_cast<DepTest*>(data)->dep1.complied();
	GEA.waitFor(h, t + Duration(4.), DepTest::freq_ready, data);
    }
}

void DepTest::dep1_ready(Handle *h, AbsTime t, void *data) {
    if (h->status == Handle::Timeout) 
	GEA.dbg() << "dep1 timeout "<< endl;
    else
	GEA.dbg() << "dep1 okay" << endl;
}


void DepTest::dep2_ready(Handle *h, AbsTime t, void *data) {
    if (h->status == Handle::Timeout) 
	GEA.dbg() << "dep2 timeout "<< endl;
    else
	GEA.dbg() << "dep2 okay" << endl;
}


DepTest::DepTest() {
    
    GEA.waitFor(&(this->blocker),  AbsTime::now() + Duration(PERIOD), 
		DepTest::blocker_ready, this);
    GEA.waitFor(&(this->freqDiv), AbsTime::now() + Duration(4.), DepTest::freq_ready, this);
    GEA.waitFor(&(this->dep1), AbsTime::now() + Duration(5.), DepTest::dep1_ready, this);
    GEA.waitFor(&(this->dep2), AbsTime::now() + Duration(4.), DepTest::dep2_ready, this);
}


extern "C" int gea_main(int argc, const char * const *argv);

int gea_main(int argc, const char * const *argv) {

    new DepTest();
    
    return 0;
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */




