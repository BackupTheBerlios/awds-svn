#include <cassert>
#include <gea/EventHandler.h>

//#include <iostream>
using namespace std;

using namespace gea;

gea::EventHandler::EventHandler() {
    //    GEA_apiIface->createSubEventHandler(this);
    // this should be done with initPosixApiIface(); 
}

gea::EventHandler::~EventHandler() {
    GEA_apiIface->destroySubEventHandler(this);
}

void gea::EventHandler::waitFor( class Handle *h, 
				 AbsTime timeout,
				 Event e,
				 void *data)
{
    this->subEventHandler->waitFor(h,timeout,e,data);
}

std::ostream& EventHandler::dbg(unsigned level) {
    return subEventHandler->dbg(level);
}


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */


