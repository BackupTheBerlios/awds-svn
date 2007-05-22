#include <gea/Handle.h>
#include <gea/ShadowHandle.h>
#include "Ns2ApiIface.h"

// gea::Handle::Handle() :
//   shadowHandle( new ShadowHandle() )
// {
    
// }

// gea::Handle::~Handle() {
//   delete shadowHandle;
// }

using namespace gea;

void Ns2ApiIface::createShadowHandle(Handle *h) {
    h->shadowHandle = new ShadowHandle();
}

void Ns2ApiIface::destroyShadowHandle(Handle *h) {
    assert(h->shadowHandle);
    delete h->shadowHandle;
    h->shadowHandle = 0;
}


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

