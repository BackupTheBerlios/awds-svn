#ifndef _TIMER_H__
#define _TIMER_H__

#include <gea/Handle.h>

namespace gea {
    
    /** \brief Handle that is always blocked.
     *  \ingroup GEA_API
     *
     *  Use this kind of handle to implement timer functionality.
     *
     *  A blocker is a special handle that is always blocked. That means 
     *  you will never be able to use read() or write() on it. However, 
     *  it will generated timeouts that can be used to generate unconditioned
     *  timers. Please consider the following example.
     *  \code 
     *  Blocker blocker;
     *
     *  void do_periodic(Handle *h, AbsTime t, void *data) {
     *      Foo *foo = static_cast<Foo *>(data);
     *      ... do something
     *      GEA.waitFor(h, t + foo->period, do_periodic, data);
     *  }
     *
     *  void start() {
     *     
     *     Foo *foo = new Foo();
     *     foo->period = gea::Duration(12.);
     *     GEA.waitFor(&blocker, AbsTime::now() + foo->period, do_periodic, data);
     *  }
     *  \endcode
     */
    class Blocker : 
	public Handle 
    {
	
    public: 
	class SubBlocker {
	    virtual ~SubBlocker();
	};
	
	SubBlocker * subBlocker;
	
	/** \brief contructor */
	Blocker();
	
	/** \brief destructor */
	virtual ~Blocker();
	
	/** \brief A write will always fail */
	virtual int write(const char *, int );
	
	/** \brief A read will always fail */
	virtual int read (char *, int );
    };
    
}

#endif //TIMER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
