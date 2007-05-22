#include <gea/Blocker.h>
#include <gea/ShadowHandle.h>
#include "Ns2ApiIface.h"

using namespace gea;

/* 
   gea::Blocker::Blocker() :
   gea::Handle(),
   shadowBlocker(0)
   {
   this->shadowHandle->handleType = gea::ShadowHandle::TypeBlocker;
   }

   gea::Blocker::~Blocker() {

   }

*/

void Ns2ApiIface::createSubBlocker(Blocker *blocker) {
    // we don't need a subBlocker.
    blocker->shadowHandle->handleType = gea::ShadowHandle::TypeBlocker;
}

void Ns2ApiIface::destroySubBlocker(Blocker *blocker) {
    /* no subBlocker created - no subBlocker destroyed */
}

/*

int gea::Blocker::write(const char *buf, int size) {
return -1;
}

int gea::Blocker::read (char *buf, int size) {
return -1;
}
*/


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
