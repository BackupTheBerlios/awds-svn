

#include <iostream>

using namespace std;

#include <common/scheduler.h>

#include <gea/ShadowEventHandler.h>

gea::EventHandler::EventHandler() :
    shadow(new gea::ShadowEventHandler())
{
    
}

gea::EventHandler::~EventHandler() {
//    delete shadow;
}

void gea::EventHandler::waitFor(gea::Handle *h, 
				gea::AbsTime timeout,
				gea::EventHandler::Event event,
				void *data) {
    
    shadow->waitFor(h, timeout, event, data);
}


std::ostream& gea::EventHandler::dbg(unsigned level) const {
    
    if (level >= this->shadow->dbgLevel ){
	
	std::cout << "[";
	std::cout.width(4);
	std::cout << shadow->currentNode->nodeid()
		  << "] ";
	std::cout.width(0);
	return std::cout;
    }
    else
	return this->shadow->nullOut;
    
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */


