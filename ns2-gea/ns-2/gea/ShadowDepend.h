#ifndef _SHADOWDEPEND_H__
#define _SHADOWDEPEND_H__


#include <common/timer-handler.h>
#include <common/scheduler.h>
#include <gea/EventHandler.h>
#include <gea/DependHandle.h>

namespace gea {
    
    class ShadowDepend : public SubDepend,  public ::TimerHandler {
	
	friend class DependHandle;
	
	class DependHandle *const parent;
	bool triggered;
	gea::EventHandler::Event e;
	void *data;
	
    public:
	
	ShadowDepend(class DependHandle *parent);
	virtual ~ShadowDepend();
	
	virtual void expire(::Event *);
	
	void activate(AbsTime t, gea::EventHandler::Event e, void *data);
	
	virtual void complied();
	
    };

}

#endif //SHADOWDEPEND_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
