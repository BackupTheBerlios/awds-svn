#ifndef _TIMER_H__
#define _TIMER_H__

#include <gea/Handle.h>

namespace gea {
    
    /** \brief Handle that is always blocked.
     *
     *  Use this kind of handle to implement timer functionality.
     */
    class Blocker : 
	public Handle 
    {
	
    public: 
	
	class SubBlocker * subBlocker;
	
	Blocker();
	virtual ~Blocker();
	
	/** \brief A write will always fail */
	virtual int write(const char *buf, int size);
	
	/** \brief A read will always fail */
	virtual int read (char *buf, int size);
    };
    
    class SubBlocker {
	virtual ~SubBlocker();
    };
}

#endif //TIMER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
