

#include <gea/Handle.h>
#include <gea/posix/ShadowHandle.h>
#include <gea/posix/PosixApiIface.h>

void gea::PosixApiIface::createShadowHandle(Handle *p_handle) {
    p_handle->shadowHandle = new ShadowHandle();
}

void gea::PosixApiIface::destroyShadowHandle(Handle *p_handle) {
    delete p_handle->shadowHandle;
    p_handle->shadowHandle = 0;
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
