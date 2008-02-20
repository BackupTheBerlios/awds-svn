
//#include <iostream>
#include <streambuf>
#include <cassert>

#include <gea/UdpHandle.h>
#include <gea/ShadowUdpHandle.h>
#include <gea/API.h>

#include <gea/ShadowHandle.h>
#include <gea/ShadowEventHandler.h>
#include <gea/EventHandler.h>
#include <gea/DependHandle.h>
#include <gea/ShadowDepend.h>
#include "Ns2ApiIface.h"

#include <iostream>

using namespace std;
using namespace gea;

/* ---- the shadow object -------- */

::TclObject *gea::ShadowEventHandler::currentNode = 0;

int gea::ShadowEventHandler::getCurrentNodeID() {
    Tcl&        tcl     = Tcl::instance();
    char buf[100];
    sprintf(buf,"%s id",currentNode->name());
    tcl.evalc(buf);
    return atoi(tcl.result()) /* +1 */; // XXX is this +1 or not?
}

class NullBuf : public std::basic_streambuf<char> {
public:
    NullBuf() : std::basic_streambuf<char>() {};
};



gea::ShadowEventHandler::ShadowEventHandler(gea::EventHandler *master) :
    SubEventHandler(master),
    nullOut(new NullBuf)
{
    char *dbg_lvl_string = getenv("GEA_DBG");
    this->dbgLevel = bool(dbg_lvl_string) ? atoi(dbg_lvl_string) : 0xFFFF;
}

gea::ShadowEventHandler::~ShadowEventHandler() {
}




void gea::ShadowEventHandler::waitFor(gea::Handle *h, 
				gea::AbsTime timeout,
				gea::EventHandler::Event event,
				void *data) {
    
    h->status = Handle::Blocked;
    
    if (h->shadowHandle->isBlocker()) {
	::Event *e = new GeaNsEvent(this, h, timeout, event, data);

	::Scheduler::instance().insert(e);
    } else if (h->shadowHandle->isDepend() ) {
	
	DependHandle *dh = dynamic_cast<DependHandle *>(h);
	ShadowDepend *shadowDepend = dynamic_cast<ShadowDepend *>(dh->subDepend);
	shadowDepend->activate(timeout, event, data);
	
    }else {
	assert(h->shadowHandle->isUdpHandle());
	gea::UdpHandle *udp = dynamic_cast<gea::UdpHandle *>(h);
	ShadowUdpHandle *shadowUdpHandle = dynamic_cast<ShadowUdpHandle *>(udp->subUdpHandle);
	if (shadowUdpHandle->mode == gea::UdpHandle::Read) {
	    shadowUdpHandle->setup_event(event, data, timeout);
	} else { // in write mode..
	    /* we allow it immediately */
	    udp->status = gea::Handle::Ready;
	    
	    event(udp, AbsTime::now(), data);
	    this->doPendingEvents(timeout);
	}
	
    }
}

void gea::ShadowEventHandler::handle(::Event* event) {
    GeaNsEvent *e = static_cast<GeaNsEvent *>(event);
    this->currentNode = e->h->shadowHandle->node;
    GEA.lastEventTime = e->timeout;
    e->event(e->h, e->timeout, e->data);
    this->doPendingEvents(e->timeout);
    delete event;
}

void gea::ShadowEventHandler::doPendingEvents(gea::AbsTime t_now) {
    
    while ( ! this->pendingList.empty() ) {
		
	PendingList todo(pendingList.size());
	
	copy(pendingList.begin(), pendingList.end(), todo.begin());
	this->pendingList.clear();
	
	for (PendingList::const_iterator i = todo.begin();
	     i != todo.end(); ++i) {
	    
	    DependHandle *dh = dynamic_cast<DependHandle*>( (*i)->h );
	    ShadowDepend *shadowDepend = dynamic_cast<ShadowDepend *>(dh->subDepend);	    
	    
	    assert( shadowDepend->status() == TIMER_PENDING );

	    shadowDepend->cancel(); // remove event from ns-2 scheduler

	    GEA.lastEventTime = t_now;
	    (*i)->e( (*i)->h,
		     t_now,
		     (*i)->data);
	    delete (*i);
	}
	
    }
}

void gea::ShadowEventHandler::addPendingEvent(gea::DependHandle *h,
					      gea::EventHandler::Event e,
					      gea::AbsTime t,
					      void *data) 
{
    this->pendingList.push_back(new EventDescr(h, data, e, t));
}

void Ns2ApiIface::createSubEventHandler(EventHandler *eh) {
    eh->subEventHandler = new ShadowEventHandler(eh);
}

void Ns2ApiIface::destroySubEventHandler(EventHandler *eh) {
    assert(eh->subEventHandler);
    delete eh->subEventHandler;
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */


