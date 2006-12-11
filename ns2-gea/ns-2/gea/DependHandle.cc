
#include <cassert>
#include <algorithm>

#include <gea/DependHandle.h>
#include <gea/ShadowDepend.h>
#include <gea/ShadowHandle.h>
#include <gea/API.h>

using namespace gea;

DependHandle::DependHandle() : 
    Handle(), 
    shadowDepend(new ShadowDepend(this) )
{
    assert (this->shadowDepend);
    this->shadowHandle->handleType = gea::ShadowHandle::TypeDepend;
}

DependHandle::~DependHandle() {
    assert (this->shadowDepend);
    delete this->shadowDepend;
}


void DependHandle::complied() {

    if (this->status != Blocked)
	return;
    this->shadowDepend->triggered = true;
    this->status = gea::Handle::Ready;
    //  this->shadowDepend->cancel();
   
    GEA.shadow->addPendingEvent(this, shadowDepend->e, AbsTime::now(), shadowDepend->data);
    
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
