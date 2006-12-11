#ifndef _EVENTHANDLER_H__
#define _EVENTHANDLER_H__

#include <ostream>

#include <gea/Handle.h>
#include <gea/Time.h>

namespace gea {

    class EventHandler {
    public:
	
	class ShadowEventHandler * const shadow;
	
	EventHandler();
	
	typedef void (*Event)(Handle *h, AbsTime t, void *data);
		
	void waitFor(Handle *h, AbsTime timeout, Event e, void *data);
	
	std::ostream& dbg(unsigned level = 0x0000FFFF) const;
	
	virtual ~EventHandler();

    };
    
}



#endif //EVENTHANDLER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
