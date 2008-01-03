#ifndef _EVENTHANDLER_H__
#define _EVENTHANDLER_H__

#include <ostream>

#include <gea/Handle.h>
#include <gea/Time.h>

namespace gea {
    
    class SubEventHandler;
    
    /** \brief This class containt the main event loop.
     *  \ingroup GEA_API
     *
     *  This class contains the main event loop. 
     *  There is one instance in the whole system - the GEA global variable.
     *  You can use it like
     *  \code
     *  GEA.waitFor(...)
     *  \endcode
     */
    class EventHandler {
    public:
	
	class SubEventHandler * subEventHandler;
	typedef void (*Event)(Handle *h, AbsTime t, void *data);
	
	/** \brief system time, the last event was triggered. 
	 *
	 *  Use this variable as GEA.lastEventTime to get the time the last
	 *  event was triggered. This is nearly the same as the current time, 
	 *  but it's more efficient. 
	 */
	gea::AbsTime lastEventTime;  
	
	
	EventHandler();
	virtual ~EventHandler();
	
	/** \brief register a new event handler
	 */ 
	virtual void waitFor(Handle *h, gea::AbsTime timeout, Event e, void *data);
	
	/** \brief ostream for debug output 
	 *
	 *  \code
	 *  GEA.dbg() << "Hello World" << std::endl;
	 *  \endcode
	 */
	virtual std::ostream& dbg(unsigned level = 0x0000FFFF) ;
	
    };

    /** \brief  this is a generic interface for pluggable switching the Event API 
     */
    class SubEventHandler {
    public:
	/** pointer to the master */
	EventHandler *masterEventHandler;
	
	/** constructor */
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
