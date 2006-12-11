#ifndef _SHADOWEVENTHANDLER_H__
#define _SHADOWEVENTHANDLER_H__

#include <common/node.h>
#include <common/scheduler.h>

#include <vector>
#include <gea/Handle.h>
#include <gea/Time.h>

#include <gea/EventHandler.h>
#include <gea/DependHandle.h>

namespace gea {
    class ShadowEventHandler : public ::Handler {
	
    public:
	
	unsigned dbgLevel;
	std::ostream nullOut;

	static Node *currentNode;
    private:
	
	struct EventDescr {
	    
	    gea::Handle *            h;
	    void *                   data;
	    gea::EventHandler::Event e;
	    gea::AbsTime             t;
	    
	    EventDescr(gea::Handle *h, 
		       void *data,
		       gea::EventHandler::Event e,
		       AbsTime t) :
		h(h),
		data(data),
		e(e),
		t(t)
	    {}
	};
	

	
	typedef vector<EventDescr *> PendingList;
	PendingList pendingList;
	
	//EventList x_eventList;
	
    public:
	ShadowEventHandler();
	
    	void waitFor(gea::Handle *h, gea::AbsTime timeout, 
		     gea::EventHandler::Event e, void *data);
	virtual void handle(::Event* event);
	/* void run(); -- not needed -- */
	virtual ~ShadowEventHandler();

	void doPendingEvents();
	void addPendingEvent(gea::DependHandle *h,
			     gea::EventHandler::Event e,
			     gea::AbsTime t,
			     void *data);

    }; // end class EventHandler

    
    /* used for scheduling events. This is usefull for blocker and similar */
    class GeaNsEvent : public ::Event {
    
    public:
	gea::Handle * const            h;
	const gea::AbsTime             timeout;
	const gea::EventHandler::Event event;
	void * const                   data;
    

	GeaNsEvent( gea::ShadowEventHandler *eh,
		    gea::Handle *      h,
		    gea::AbsTime       timeout,
		    gea::EventHandler::Event event,
		    void *                   data) :
	    h(h),
	    timeout(timeout),
	    event(event),
	    data(data)
	
	{
	    this->handler_ = eh;
	    this->time_ = static_cast<double>(timeout - gea::AbsTime::t0());
	}
    
    };
    
    


}; // end namespace gea



#endif //SHADOWEVENTHANDLER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
