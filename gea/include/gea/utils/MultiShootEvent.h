#ifndef __MULTISHOOTEVENT_H_
#define __MULTISHOOTEVENT_H_

#include <gea/API.h>

namespace gea {

    class MultiShootEvent {
    public:
	bool alive;
    public:
	MultiShootEvent() : alive(true) {};
	virtual ~MultiShootEvent() {};

	virtual void handle(Handle *h, AbsTime t) = 0;

    protected:
	static void event(Handle *h, AbsTime t, void *data);

	void waitagain() {alive = true;}
    };

};


#endif // __MULTISHOOTEVENT_H_
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
