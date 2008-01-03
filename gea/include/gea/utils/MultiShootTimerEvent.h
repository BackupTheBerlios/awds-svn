#ifndef __MULTISHOOTTIMEREVENT_H_
#define __MULTISHOOTTIMEREVENT_H_

#include "gea/utils/MultiShootEvent.h"

namespace gea {

    class MultiShootTimerEvent : public MultiShootEvent {
    protected:
	Blocker blocker;
	Duration duration;
	unsigned jitter;
	double jitterbase;
	void wait() {GEA.waitFor(&blocker,AbsTime::now() + duration, MultiShootTimerEvent::event, this);}
    protected:
	MultiShootTimerEvent(Duration d) : duration(d), jitter(0), jitterbase(0) {};
	static void event(Handle *h, AbsTime t, void *data);
	void setJitter(unsigned j, double b) {jitter = j; jitterbase = b;}
    };

};

#endif // __MULTISHOOTTIMEREVENT_H_
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
