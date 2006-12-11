#ifndef __SINGLESHOOTEVENT_H_
#define __SINGLESHOOTEVENT_H_

#include <gea/API.h>

namespace gea {

    class SingleShootEvent {
    protected:
	Handle *wh;
    public:
	SingleShootEvent(Handle &handle, Duration timeout) : wh(&handle) {};
	SingleShootEvent(Handle &handle, AbsTime timeout) : wh(&handle) {};

	virtual ~SingleShootEvent()  {};
	
	virtual void handle(Handle *h, AbsTime t) = 0;
	
    protected:
	static void event(Handle *h, AbsTime t, void *data);
	void wait(AbsTime timeout);
    };

};


#endif // __SINGLESHOOTEVENT_H_
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
