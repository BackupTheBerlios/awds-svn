
#ifdef HAVE_SYS_SELECT_H
/* According to POSIX 1003.1-2001 */
#include <sys/select.h>
#endif // HAVE_SYS_SELECT_H
#include <cerrno>
#include <cassert>


#include <streambuf>

#include <math.h>

#include <cstdlib>
#include <gea/posix/ShadowEventHandler.h>

#include <gea/ObjRepository.h>

#include <gea/DependHandle.h>
#include <gea/UdpHandle.h>
#include <gea/posix/ShadowHandle.h>
#include <gea/posix/ShadowUdpHandle.h>
#include <gea/posix/ShadowEventHandler.h>
#include <gea/posix/UnixFdHandle.h>
#include <gea/posix/PosixApiIface.h>

#include <gea/gea_main.h>

class NullBuf : public std::basic_streambuf<char> {
public:
    NullBuf() : std::basic_streambuf<char>() {};
};



gea::ShadowEventHandler::ShadowEventHandler(EventHandler *master) :
    SubEventHandler(master),
    nullOut(new NullBuf())
{
    
    char *dbg_lvl_string = getenv("GEA_DBG");
    this->dbgLevel = bool(dbg_lvl_string) ? atoi(dbg_lvl_string) : 0xFFFF;
}

gea::ShadowEventHandler::~ShadowEventHandler() {
    
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
	    
	    gea::UnixFdHandle *uh;
	    if ( i->second.h->shadowHandle->isUnixFdHandle() )
		uh = static_cast<gea::UnixFdHandle *>(i->second.h);
	    else
		uh = dynamic_cast<gea::UnixFdHandle *>(static_cast<gea::UdpHandle *>(i->second.h)->subUdpHandle);
	    
	    switch ( uh->shadowHandle->unixMode ) {
	    case gea::PosixModeRead :
		FD_SET(uh->shadowHandle->unixFd, set[0]);
		break;
	    case gea::PosixModeWrite :
		FD_SET(uh->shadowHandle->unixFd, set[1]);
		break;
	    case gea::PosixModeException :
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

typedef std::vector<EventCall> EventCallList; 


/** operator for calling the events in the list */
struct callECList {
    gea::AbsTime& lastTime;
    
    callECList(gea::AbsTime& lt) : lastTime(lt) {}
    
    void operator ()(EventCall& ec) const {
	lastTime = ec.t;
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
	    
	    gea::UnixFdHandle *uh;
	    if ( i.second.h->shadowHandle->isUnixFdHandle() )
		uh =  static_cast<gea::UnixFdHandle *>(i.second.h) ;
	    else 
		uh = dynamic_cast<gea::UnixFdHandle *>(static_cast<gea::UdpHandle *>(i.second.h)->subUdpHandle);
	    
	    
	    if ( FD_ISSET(uh->shadowHandle->unixFd, set[uh->shadowHandle->unixMode]) ) { 
		// found active fd !
		i.second.h->status = gea::Handle::Ready;
		
		EventCall ec;
		ec.h = i.second.h;
		ec.t = gea::AbsTime::now();
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
    
    typedef std::vector<gea::DependHandle*> ResetList;
    
    EventCallList& elist;
    activeDepPred(EventCallList& elist) : elist(elist) {}

    bool operator () (gea::ShadowEventHandler::EventList::value_type &i) const {
	if ( i.second.h->shadowHandle->isDepend() ) {
	    gea::DependHandle *d = dynamic_cast<gea::DependHandle *>(i.second.h);
	    gea::ShadowDepend *shadowDepend = dynamic_cast<gea::ShadowDepend *>(d->subDepend);
	    if (shadowDepend->triggered) {
		i.second.h->status = gea::Handle::Ready;
		
		EventCall ec;
		ec.h = i.second.h;
		ec.t = gea::AbsTime::now();
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
	gea::DependHandle *dh = static_cast<gea::DependHandle *>(ec.h);
	gea::ShadowDepend *shadowDepend = dynamic_cast<gea::ShadowDepend *>(dh->subDepend);
	shadowDepend->triggered = false;
    }
    
};






DLLEXPORT void gea::ShadowEventHandler::run() {
    

    while (! eventList.empty() ) {
/*	
	EventDescr ed = this->eventList.begin()->second;
*/	
	gea::Duration sleepTime = this->eventList.begin()->first - gea::AbsTime::now();
	
	if (sleepTime < gea::Duration(0))
	    sleepTime = gea::Duration(0);
	
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
	    
	    this->masterEventHandler->lastEventTime = eventList.begin()->first; // store event time for later use
	    
	    eventList.begin()->second.e(eventList.begin()->second.h,
					eventList.begin()->first,
					eventList.begin()->second.data);
	    
	    eventList.erase(eventList.begin());
	  
	    
	} else {
	    /* handle all io events */
	    EventCallList elist;
	    remove_if(eventList, activeFdHandlePred(set, elist));
	    for_each(elist.begin(), elist.end(), callECList(masterEventHandler->lastEventTime) );
	}


	/* handle all dependend events */
	
	EventCallList elist;
	assert(elist.size() == 0);
		
	while( remove_if(eventList, activeDepPred(elist)) ) {
	    
	    assert(elist.size() > 0);
	    
	    for_each(elist.begin(), elist.end(), callECList(masterEventHandler->lastEventTime) );
	    for_each(elist.begin(), elist.end(), resetDepTrigger() );
	    
	    elist.clear();
	    assert(elist.size() == 0);
	    
	}
    } // end while more events
}

void gea::ShadowEventHandler::waitFor( gea::Handle *h,
				       gea::AbsTime timeout, 
				       gea::EventHandler::Event e, void *data) 
{
    
    h->status = gea::Handle::Blocked;
    
    EventDescr ed( h, data, e);
    
    assert(e);
    eventList.insert(EventList::value_type(timeout, ed) );
}


std::ostream& gea::ShadowEventHandler::dbg(unsigned level) {
    
    REP_MAP_OBJ(std::ostream**, GEA_defaultOstream);
    
    if (level >= this->dbgLevel )
	return **GEA_defaultOstream; 
    else
	return this->nullOut;
}

static std::ostream* __x_defaultOstream = &std::cout;

void gea::PosixApiIface::createSubEventHandler(gea::EventHandler *eh) {
    REP_INSERT_OBJ(std::ostream**, GEA_defaultOstream, &__x_defaultOstream);
    eh->subEventHandler = new ShadowEventHandler(eh);
}

void gea::PosixApiIface::destroySubEventHandler(gea::EventHandler *eh) {
    delete eh->subEventHandler;
}

long gea::PosixApiIface::getCurrentNode() {
    return 0;
}

#include <gea/API.h>

DLLEXPORT void gea::initPosixApiIface() {
    GEA_apiIface = new gea::PosixApiIface();
    GEA_apiIface->createSubEventHandler(&GEA);
    GEA.lastEventTime = gea::AbsTime::now();
}


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

