#ifndef _EVENTHANDLER_H__
#define _EVENTHANDLER_H__

#include <ostream>

#include <gea/Handle.h>
#include <gea/Time.h>

namespace gea {

    class EventHandler {
    public:
	
	class ShadowEventHandler * const shadow;
	typedef void (*Event)(Handle *h, AbsTime t, void *data);

	gea::AbsTime lastEventTime; 

	EventHandler();
	virtual ~EventHandler();
	
	virtual void waitFor(Handle *h, gea::AbsTime timeout, Event e, void *data);
	virtual std::ostream& dbg(unsigned level = 0x0000FFFF) const;
	
    };
    
}



#endif //EVENTHANDLER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
