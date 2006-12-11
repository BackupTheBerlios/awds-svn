#ifndef _DEPEND_H__
#define _DEPEND_H__


#include <gea/Handle.h>

namespace gea {

    class DependHandle: public Handle {
	
   
    public:
	
	class ShadowDepend * const shadowDepend;
	
	DependHandle();
	virtual ~DependHandle();
	void complied();
	
    	virtual int write(const char *buf, int size) { return -1; }
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
