
//#include <iostream>
#include <streambuf>
#include <cassert>

#include <gea/UdpHandle.h>
#include <gea/ShadowUdpHandle.h>

#include <gea/ShadowHandle.h>
#include <gea/ShadowEventHandler.h>
#include <gea/EventHandler.h>
#include <gea/DependHandle.h>
#include <gea/ShadowDepend.h>

#include <iostream>

using namespace std;


/* ---- the shadow object -------- */

// <begin> jenz::inria
// ::Node * gea::ShadowEventHandler::currentNode = 0;
::TclObject *gea::ShadowEventHandler::currentNode = 0;
// <end> jenz::inria

// <begin> jenz::inria
int gea::ShadowEventHandler::getCurrentNodeID() {
    Tcl&        tcl     = Tcl::instance();
    char buf[100];
    sprintf(buf,"%s id",currentNode->name());
    tcl.evalc(buf);
    return atoi(tcl.result());
}
// <end> jenz::inria

class NullBuf : public std::basic_streambuf<char> {
public:
    NullBuf() : std::basic_streambuf<char>() {};
};



gea::ShadowEventHandler::ShadowEventHandler() :
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
	//	cout << "DBG waitfor " << dh->shadowDepend << endl;
	
	dh->shadowDepend->activate(timeout, event, data);
	
    }else {
	assert(h->shadowHandle->isUdpHandle());
	gea::UdpHandle *udp = dynamic_cast<gea::UdpHandle *>(h);

	if (udp->shadowUdpHandle->mode == gea::UdpHandle::Read) {
	    udp->shadowUdpHandle->setup_event(event, data, timeout);
	} else { // in write mode..
	    /* we allow it immediately */
	    udp->status = gea::Handle::Ready;
	    
	    event(udp, AbsTime::now(), data);
	    this->doPendingEvents();
	}
	
    }
}

void gea::ShadowEventHandler::handle(::Event* event) {
    GeaNsEvent *e = static_cast<GeaNsEvent *>(event);
    this->currentNode = e->h->shadowHandle->node;
    e->event(e->h, e->timeout, e->data);
    this->doPendingEvents();
    delete event;
}

void gea::ShadowEventHandler::doPendingEvents() {
    
    while ( ! this->pendingList.empty() ) {
		
	PendingList todo(pendingList.size());
	
	copy(pendingList.begin(), pendingList.end(), todo.begin());
	this->pendingList.clear();
	
	for (PendingList::const_iterator i = todo.begin();
	     i != todo.end(); ++i) {
	    
	    DependHandle *dh = dynamic_cast<DependHandle*>( (*i)->h );
	    
	    assert( dh->shadowDepend->status() == TIMER_PENDING );
	    	    
	    dh->shadowDepend->cancel(); // remove event from ns-2 scheduler
	    (*i)->e( (*i)->h,
		     (*i)->t,
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

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */


