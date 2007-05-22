

#include <iostream>

using namespace std;

#include <common/scheduler.h>

#include <gea/ShadowEventHandler.h>


std::ostream& gea::ShadowEventHandler::dbg(unsigned level)  {
    
    if (level >= this->dbgLevel ){
	
	std::cout << "[";
	std::cout.width(4);
	// <begin> jenz::inria
	//	std::cout << shadow->currentNode->nodeid()	    
	//		  << "] ";
	std:: cout << this->getCurrentNodeID() << "] ";
	// <end> jenz::inria
	std::cout.width(0);
	return std::cout;
    }
    else
	return this->nullOut;
    
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */


