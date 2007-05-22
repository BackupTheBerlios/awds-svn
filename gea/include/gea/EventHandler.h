#ifndef _EVENTHANDLER_H__
#define _EVENTHANDLER_H__

#include <ostream>

#include <gea/Handle.h>
#include <gea/Time.h>

namespace gea {
    
    class SubEventHandler;
    
    /* this class represents the main API. */
    class EventHandler {
    public:
	
	class SubEventHandler * subEventHandler;
	typedef void (*Event)(Handle *h, AbsTime t, void *data);

	gea::AbsTime lastEventTime; 

	EventHandler();
	virtual ~EventHandler();
	
	virtual void waitFor(Handle *h, gea::AbsTime timeout, Event e, void *data);
	virtual std::ostream& dbg(unsigned level = 0x0000FFFF) ;
	
    };

    /* this is a generic interface for pluggable switching the Event API */
    class SubEventHandler {
    public:
	EventHandler *masterEventHandler;
	
	SubEventHandler(EventHandler *master) : masterEventHandler(master) {}
	virtual void waitFor(Handle *h, gea::AbsTime timeout, gea::EventHandler::Event e, void *data) = 0;
	virtual std::ostream& dbg(unsigned level = 0x0000FFFF)  = 0;
	virtual ~SubEventHandler() {}
    };
    
    
}



#endif //EVENTHANDLER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
