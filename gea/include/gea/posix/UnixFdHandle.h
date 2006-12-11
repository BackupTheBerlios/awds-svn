#ifndef _UNIXFDHANDLER_H__
#define _UNIXFDHANDLER_H__


#include <gea/posix/ShadowHandle.h>
#include <gea/Handle.h>

namespace gea {

    class UnixFdHandle : public Handle {
	
    public:
	
	UnixFdHandle(int fd, gea::ShadowHandle::UnixMode mode);
	
	virtual int write(const char *buf, int size);
	virtual int read (char *buf, int size);
	
	~UnixFdHandle();
    };
    
}


#endif //UNIXFDHANDLER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
