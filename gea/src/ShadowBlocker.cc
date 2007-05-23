
#include <gea/posix/PosixApiIface.h>
#include <gea/Blocker.h>
#include <gea/posix/ShadowHandle.h>

using namespace gea;

void PosixApiIface::createSubBlocker(Blocker *b) {
    //    b->shadowBlocker = 0;
    b->shadowHandle->handleType = gea::ShadowHandle::TypeBlocker;
}

void PosixApiIface::destroySubBlocker(Blocker *b) {
    
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
