#ifndef _SHADOWEVENTHANDLER_H__
#define _SHADOWEVENTHANDLER_H__

#include <map>
#include <fstream>

#include <gea/EventHandler.h>
#include <gea/posix/ShadowDependHandle.h>


namespace gea {


    
    class ShadowEventHandler {
    protected:

	
    public:
    
	struct EventDescr {
	    class Handle *h;
	    void *data;
	    EventHandler::Event e;


	    EventDescr(Handle *h, 
		       void *data,
		       EventHandler::Event e) :
		h(h),
		data(data),
		e(e)
	    {}
	};
	
	typedef std::multimap<AbsTime, EventDescr> EventList;


    private:
	unsigned iter;
	EventList eventList;

	friend class DependHandle;
	gea::ShadowDepend::PendList pendList;
	
    public:
	unsigned dbgLevel;
	std::ostream nullOut;
	
	ShadowEventHandler();
	~ShadowEventHandler();

	void waitFor(Handle *h, AbsTime timeout,
		     gea::EventHandler::Event e, void *data);
	void run();
	

    };
}

#endif //SHADOWEVENTHANDLER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
