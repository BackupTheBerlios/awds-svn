#include <iostream>

#include <gea/Handle.h>
#include <gea/ShadowDepend.h>
#include <gea/DependHandle.h>
#include <gea/API.h>

using namespace gea;
using namespace std;

ShadowDepend::ShadowDepend(DependHandle *parent) :
    TimerHandler(),
    parent(parent)
{
    
}

ShadowDepend::~ShadowDepend() {
    
}


void ShadowDepend::expire(::Event *) {
  
    this->parent->status = gea::Handle::Timeout;
    GEA.lastEventTime = AbsTime::now();
    this->e(this->parent, GEA.lastEventTime, this->data);
    
}


void ShadowDepend::activate(AbsTime t, 
			    gea::EventHandler::Event e,
			    void *data) 
{
   
    this->e = e;
    this->data = data;
    assert(this->status() != TIMER_PENDING);
  
    this->resched(t - AbsTime::now());
    assert(this->status() == TIMER_PENDING);
  
}


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
