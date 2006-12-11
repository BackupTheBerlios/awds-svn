
#include <cassert>
#include <gea/DependHandle.h>
#include <gea/posix/ShadowHandle.h>
#include <gea/posix/ShadowEventHandler.h>
#include <gea/posix/ShadowDependHandle.h>
#include <gea/API.h>


gea::DependHandle::DependHandle() :
    Handle(), 
    shadowDepend(new ShadowDepend())
{
    assert(this->shadowDepend);
    this->shadowHandle->handleType = gea::ShadowHandle::TypeDepend;
}

gea::DependHandle::~DependHandle() {
    
    if (this->shadowDepend) 
	delete this->shadowDepend;
    
}

void gea::DependHandle::complied() {
    
    //  this->shadowDepend->pendList.moveTo(GEA.shadow->pendList);
    this->shadowDepend->triggered = true;
    this->status = Handle::Ready;
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
