
#include <gea/posix/ShadowDependHandle.h>
#include <gea/posix/PosixApiIface.h>
#include <gea/posix/ShadowHandle.h>

void gea::ShadowDepend::complied() {

    //  this->shadowDepend->pendList.moveTo(GEA.shadow->pendList);
    this->triggered = true;
    this->master->status = Handle::Ready;
}

void gea::PosixApiIface::createSubDepend(DependHandle *dh) {
    dh->shadowHandle->handleType = gea::ShadowHandle::TypeDepend;
    dh->subDepend = new ShadowDepend(dh);
}

void gea::PosixApiIface::destroySubDepend(DependHandle *dh) {
    assert (dh->subDepend);
    delete dh->subDepend;
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
