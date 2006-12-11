#ifndef __SINGLESHOOTTIMEREVENT_H_
#define __SINGLESHOOTTIMEREVENT_H_

#include "gea/utils/SingleShootEvent.h"

#include <iostream>
using namespace std;

namespace gea {
    
    class SingleShootTimerEvent : SingleShootEvent {
    protected:
	Blocker blocker;
	
// 	void wait(Duration d) {
// 	    cout << "SSE(" << (void*)&GEA << ":" << (void*)&blocker << ", " << (void*)this << endl; 
// 	    GEA.waitFor(&blocker,AbsTime::now() + d, SingleShootEvent::event, this);
// 	}
    public:
	SingleShootTimerEvent(Duration timeout) : SingleShootEvent(blocker, timeout) {
	    wait(AbsTime::now() + timeout);
	}
	SingleShootTimerEvent(AbsTime timeout) : SingleShootEvent(blocker, timeout) {
	    wait(timeout);
	}
    };

};

#endif // __SINGLESHOOTTIMEREVENT_H_
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
