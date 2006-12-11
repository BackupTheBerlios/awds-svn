#ifndef _SHADOWDEPENDHANDLE_H__
#define _SHADOWDEPENDHANDLE_H__

#include <queue>

#include <gea/EventHandler.h>

namespace gea {
    
    class ShadowDepend {
    public:

	
	bool triggered;
	
	struct PendEvent {
	    class PendEvent    *next;
	    class Handle       *h;
	    EventHandler::Event e;
	    void *              data;
	    AbsTime             t;
	};
    
	class PendList {
	protected:
	    PendEvent *first;
	    PendEvent *last;
	public:
	    
	    PendList() : first(0), last(0) {}
	    ~PendList() {
		while (first != 0) {
		    PendEvent *tmp = first;
		    first = first->next;
		    delete tmp;
		}		
	    }
	    
	    bool isEmpty() const { return first == 0; }
	    
	    bool append(class Handle *h,
			EventHandler::Event e,
			void *data,
			AbsTime t) {
		PendEvent *p = new PendEvent();
		if (p == 0) return false;
		
		p->h = h; p->e = e; p->data = data; p->t = t;
		p->next = 0;
		if (isEmpty() ) {
		    first = p;
		} else {
		    last->next = p;
		}
		last = p;
		return true;
	    }

	    void moveTo(PendList &pl) {
		if (pl.isEmpty()) {
		    pl.first = this->first;
		} else {
		    pl.last->next = this->first;
		}
		pl.last = this->last;
		this->first = 0;
		this->last  = 0;
		
	    }
	    
	    PendEvent *pop() {
		PendEvent *ret = first;
		if (ret != 0) {
		    first = first->next;
		}
		return ret;
	    }
	    
	};
	
       
	//	PendList pendList;
	
	ShadowDepend() : triggered(false) {}

    };


}

#endif //SHADOWDEPENDHANDLE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
