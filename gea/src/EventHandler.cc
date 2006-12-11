#include <config.h>



#ifdef HAVE_SYS_SELECT_H
/* According to POSIX 1003.1-2001 */
#include <sys/select.h>
#endif // HAVE_SYS_SELECT_H

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <math.h>
#include <cerrno>
#include <cassert>

#include <gea/DependHandle.h>
#include <gea/UdpHandle.h>
#include <gea/posix/ShadowHandle.h>
#include <gea/posix/ShadowUdpHandle.h>
#include <gea/posix/ShadowEventHandler.h>
#include <gea/posix/UnixFdHandle.h>
#include <gea/Handle.h>

#include <gea/API.h>

#include <iostream>
using namespace std;

using namespace gea;

gea::EventHandler::EventHandler() :
    shadow(new ShadowEventHandler() )
{
    
}

gea::EventHandler::~EventHandler() {
    delete this->shadow;
}


void gea::EventHandler::waitFor( 
				class Handle *h, 
				AbsTime timeout,
				Event e,
				void *data)
{
    assert(e);
    this->shadow->waitFor(h,timeout,e,data);
}

void 
gea::ShadowEventHandler::waitFor(
				      gea::Handle *h,
				      gea::AbsTime timeout, 
				      gea::EventHandler::Event e, void *data) {
    
    h->status = gea::Handle::Blocked;
    
    EventDescr ed( h, data, e);
    
    assert(e);
    eventList.insert(EventList::value_type(timeout, ed) );
}


    
static void double2Timeval(double t, struct timeval *tv) {
    tv->tv_sec = static_cast<int>( floor(t) );
    tv->tv_usec = static_cast<int>( (t - floor(t)) * 1000000.);
}


/** function for filling the sets used by select */
static void fill_fd_sets(gea::ShadowEventHandler::EventList::const_iterator from, 
			 gea::ShadowEventHandler::EventList::const_iterator to,
			 fd_set **set,
			 gea::ShadowHandle::fd_handle_t &max_fd ) {
    
    for (int i = 0; i < 3; ++i) {
	FD_ZERO(set[i]);
    }
    max_fd = 0;
        
    for (gea::ShadowEventHandler::EventList::const_iterator i  = from;
	 i != to;
	 i++) {

	
	if ( i->second.h->shadowHandle->isUnixFdHandle()
	     || i->second.h->shadowHandle->isUdpHandle() ) {
	    
	    UnixFdHandle *uh = i->second.h->shadowHandle->isUnixFdHandle() ? 
		static_cast<UnixFdHandle *>(i->second.h) :
		static_cast<UdpHandle *>(i->second.h)->shadowUdpHandle;
	    
	    switch ( uh->shadowHandle->unixMode ) {
	    case gea::ShadowHandle::Read:

		FD_SET(uh->shadowHandle->unixFd, set[0]);
		break;
	    case gea::ShadowHandle::Write:
		FD_SET(uh->shadowHandle->unixFd, set[1]);
		break;
	    case gea::ShadowHandle::Exception:
		FD_SET(uh->shadowHandle->unixFd, set[2]);
		break;
	    default:
		assert(!"undefined handle mode");
	    }
	    
	    max_fd = (max_fd > uh->shadowHandle->unixFd) ? max_fd : uh->shadowHandle->unixFd;
	} // end if  i->second.h->getType() == UnixFdHandle::TypeName
	
    } // end for all events..
    
}

struct EventCall {
    gea::AbsTime t;
    gea::Handle *h;
    gea::EventHandler::Event e;
    void *d;
};

typedef vector<EventCall> EventCallList; 


/** operator for calling the events in the list */
struct callECList {
    void operator ()(EventCall& ec) const {
	
	ec.e(ec.h, ec.t, ec.d );
	
    }
};

struct activeFdHandlePred {
    
    fd_set const * const *set;
    EventCallList& elist;

    activeFdHandlePred(fd_set const * const *_set, EventCallList& elist) :
	set(_set),
	elist(elist)
    { }
	
    bool operator () (gea::ShadowEventHandler::EventList::value_type &i) const {
    	if  ( i.second.h->shadowHandle->isUnixFdHandle() 
	      || i.second.h->shadowHandle->isUdpHandle() ) {
	    
	    UnixFdHandle *uh = i.second.h->shadowHandle->isUnixFdHandle() 
		? static_cast<UnixFdHandle *>(i.second.h) 
		: static_cast<UdpHandle *>(i.second.h)->shadowUdpHandle;
	    
	    if ( FD_ISSET(uh->shadowHandle->unixFd, set[uh->shadowHandle->unixMode]) ) { 
		// found active fd !
		i.second.h->status = Handle::Ready;
		
		EventCall ec;
		ec.h = i.second.h;
		ec.t = AbsTime::now();
		ec.e = i.second.e;
		ec.d = i.second.data;
		

		elist.push_back(ec);
		return true; // ... and remove
	    }
	}	
	return false; // ... and remove 
    }
    
};

/** predicate for testing of active dependencies.
 *  all events are inserted into the event call list for later execution.
 */
struct activeDepPred {
    
    typedef vector<DependHandle*> ResetList;
    
    EventCallList& elist;
    activeDepPred(EventCallList& elist) : elist(elist) {}

    bool operator () (gea::ShadowEventHandler::EventList::value_type &i) const {
	if ( i.second.h->shadowHandle->isDepend() ) {
	    DependHandle *d = dynamic_cast<DependHandle *>(i.second.h);

	    if (d->shadowDepend->triggered) {
		i.second.h->status = Handle::Ready;
		
		EventCall ec;
		ec.h = i.second.h;
		ec.t = AbsTime::now();
		ec.e = i.second.e;
		ec.d = i.second.data;
				
		elist.push_back(ec);
		
		return true; // ... and remove 
	    }			
	}
	return false;
    }
};
  
/** modified remove_if for use with map<> */
template <class Pred>
static bool remove_if(gea::ShadowEventHandler::EventList& eventList, const Pred& pred) {
    
    bool modified = false;
    
    gea::ShadowEventHandler::EventList::iterator i = eventList.begin();
    gea::ShadowEventHandler::EventList::iterator iplus1 = i;
    if (i != eventList.end()) ++iplus1;
	    
    while ( i != eventList.end() ) {
	if ( pred(*i) ) {
	    eventList.erase(i);
	    modified = true;
	}
	i = iplus1;
	if ( iplus1 != eventList.end() ) ++iplus1;
    }
    
    return modified;
}
  
/** operator for resetting the internal state */
struct resetDepTrigger {
    
    void operator ()(EventCall& ec) {
	assert(ec.h->shadowHandle->isDepend());
	DependHandle *dh = static_cast<DependHandle *>(ec.h);
	dh->shadowDepend->triggered = false;}
    
};

void ShadowEventHandler::run() {
    

    while (! eventList.empty() ) {
/*	
	EventDescr ed = this->eventList.begin()->second;
*/	
	Duration sleepTime = this->eventList.begin()->first - AbsTime::now();
	
	if (sleepTime < Duration(0.))
	    sleepTime = Duration(0.);
	
	struct timeval tv;
	double2Timeval(static_cast<double>(sleepTime), &tv);

	// collect all unix io events 
	
	fd_set rset;
	fd_set wset;
	fd_set eset;

	fd_set *set[] = {&rset, &wset, &eset};

	gea::ShadowHandle::fd_handle_t max_fd;
	
	fill_fd_sets(this->eventList.begin(),
		     this->eventList.end(),
		     set,
		     max_fd);
	
	int sel_ret;
	do {
	    sel_ret = ::select(max_fd + 1, &rset, &wset, &eset, &tv);
	} while ( ( sel_ret == -1 ) && (errno == EINTR));
	if (sel_ret == -1) {
	    perror("GEA event loop (select system call): ");
	    assert (sel_ret != -1);
	}
	if (sel_ret == 0) { // timeout
	    // must be the first one..
	   
	    eventList.begin()->second.h->status = Handle::Timeout;
	    
	    eventList.begin()->second.e(eventList.begin()->second.h,
					eventList.begin()->first,
					eventList.begin()->second.data);
	    
	    eventList.erase(eventList.begin());
	  
	    
	} else {
	    /* handle all io events */
	    EventCallList elist;
	    remove_if(eventList, activeFdHandlePred(set, elist));
	    for_each(elist.begin(), elist.end(), callECList() );
	}


	/* handle all dependen events */
	
	EventCallList elist;
	assert(elist.size() == 0);
		
	while( remove_if(eventList, activeDepPred(elist)) ) {
	    
	    assert(elist.size() > 0);
	    
	    for_each(elist.begin(), elist.end(), callECList() );
	    for_each(elist.begin(), elist.end(), resetDepTrigger() );
	    
	    elist.clear();
	    assert(elist.size() == 0);
	    
	}
    } // end while more events
}





std::ostream& EventHandler::dbg(unsigned level) const {
    
    if (level >= this->shadow->dbgLevel )
	return std::cout;
    else
	return this->shadow->nullOut;
    
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */


