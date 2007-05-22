
#include <cassert>
#include <gea/DependHandle.h>
#include <gea/posix/ShadowHandle.h>
#include <gea/posix/ShadowEventHandler.h>
#include <gea/posix/ShadowDependHandle.h>
#include <gea/API.h>
#include <gea/ApiIface.h>

gea::DependHandle::DependHandle() :
    Handle() 
{
    GEA_apiIface->createSubDepend(this);
}

gea::DependHandle::~DependHandle() {
    GEA_apiIface->createSubDepend(this);
}


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
