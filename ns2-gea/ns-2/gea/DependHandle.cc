
#include <cassert>
#include <algorithm>


#include <gea/DependHandle.h>
#include <gea/ShadowDepend.h>
#include <gea/ShadowHandle.h>
#include <gea/API.h>

#include "Ns2ApiIface.h"

using namespace gea;


void Ns2ApiIface::createSubDepend(DependHandle *dh) {
    dh->shadowHandle->handleType = gea::ShadowHandle::TypeDepend;
    dh->subDepend = new ShadowDepend(dh);
    dh->subDepend->master = dh;
}


void Ns2ApiIface::destroySubDepend(DependHandle *dh) {
    assert(dh->subDepend);
    delete dh->subDepend;
}



// DependHandle::~DependHandle() {
// assert (this->shadowDepend);
// delete this->shadowDepend;
// }

void ShadowDepend::complied() {

    if (master->status != Handle::Blocked)
	return;
    this->triggered = true;
    this->master->status = gea::Handle::Ready;
    
    ShadowEventHandler *shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler);
    shadow->addPendingEvent(this->master, this->e, GEA.lastEventTime, this->data);
    
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
