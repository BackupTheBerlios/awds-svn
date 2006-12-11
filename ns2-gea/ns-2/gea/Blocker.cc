#include <gea/Blocker.h>
#include <gea/ShadowHandle.h>

gea::Blocker::Blocker() :
    gea::Handle(),
    shadowBlocker(0)
{
    this->shadowHandle->handleType = gea::ShadowHandle::TypeBlocker;
}

gea::Blocker::~Blocker() {
    
}


int gea::Blocker::write(const char *buf, int size) {
    return -1;
}

int gea::Blocker::read (char *buf, int size) {
    return -1;
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
