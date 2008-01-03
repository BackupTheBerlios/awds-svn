#ifndef _DEPEND_H__
#define _DEPEND_H__


#include <gea/Handle.h>

namespace gea {


    class SubDepend {
    public:
	
	class DependHandle *master;
	virtual ~SubDepend() {}
	virtual void complied() = 0;
    };

    
    /** \brief %Handle for waiting on user-generated events.
     *  \ingroup GEA_API
     * 
     *  \todo add detailed comments.
    */
    class DependHandle: public Handle {
	
   
    public:
	
	class SubDepend *  subDepend;
	
	DependHandle();
	virtual ~DependHandle();
	
	/** \brief trigger the handle and wake up all waiting event handlers */
	void complied() { subDepend->complied(); }
	
    	/** \brief dummy function that never succeeds */
	virtual int write(const char *buf, int size) { return -1; }
	
	/** \brief dummy function that never succeeds */
	virtual int read (char *buf,       int size) { return -1; }
	
    };
    
}

#endif //DEPEND_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
