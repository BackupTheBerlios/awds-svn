#ifndef __FlowTracker_H_
#define __FlowTracker_H_

namespace gea {

    /// base class to track a flow and to react if its finishes
    class FlowTracker {
    protected:
	/// number of events in this flow
	unsigned count;
	
    public:
	FlowTracker() : count (0) {};
	virtual  ~FlowTracker() {};
	
	unsigned getCount() const {return count;}

	void inc() {count++;}
	void dec() {count--; if (!count) action();}

	virtual void action() {};
    };

}
	
#endif // __FlowTracker_H_

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
