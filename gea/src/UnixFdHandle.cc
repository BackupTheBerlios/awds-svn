#include <unistd.h>
#include <cassert>

#include <gea/posix/ShadowHandle.h>
#include <gea/posix/UnixFdHandle.h>
#include <gea/gea_main.h>

using namespace gea;


UnixFdHandle::UnixFdHandle(int fd, gea::ShadowHandle::UnixMode mode) 
{
    this->shadowHandle->handleType = ShadowHandle::TypeUnixFdHandle;
    this->shadowHandle->unixMode = mode;
    this->shadowHandle->unixFd = fd;
}

DLLEXPORT UnixFdHandle::~UnixFdHandle() {
    this->shadowHandle->unixFd = 0;
}


// const char * UnixFdHandle::TypeName = "UnixFdHandler";

// const char *UnixFdHandle::getType() const {
//     return UnixFdHandle::TypeName; 
// }

int UnixFdHandle::write(const char *buf, int size) {
    assert (status == Ready);
    assert (this->shadowHandle->isUnixFdHandle());
    return ::write(this->shadowHandle->unixFd, buf, size);
}

int UnixFdHandle::read (char *buf, int size) {
    assert (status == Ready);
    return ::read(this->shadowHandle->unixFd, buf, size);
    
}
  

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

