
/** This is the ns-2 implementation of AnimLink.
 *  It is used  for creating animation commands in the nam file.
 *
 *  (c) 2005 Andre Herms <aherms@ivs.cs.uni-magdeburg.de>
 */

#include <gea/AnimLink.h>
#include <gea/API.h>
#include <gea/ShadowEventHandler.h>

#include <common/simulator.h>


static void inOutLink(bool in, int dest, const char *color = 0) {
  
    
  // int namid = GEA.shadow->getCurrentNodeID();
  int namid = GEA_apiIface->getCurrentNode();
  
  if (namid == dest) // no links to ourself
    return;
  
  double      time    = Scheduler::instance().clock();
  const char *simName = Simulator::instance().name();
  Tcl&        tcl     = Tcl::instance();
  
  tcl.evalf( "%s puts-nam-config \"l -t %f -s %d "
	     "-S %s "/* in or out*/
	     "%s %s -d %d\"", 
	     simName, time, namid,
	     in ? "in" : "out",
	     in ? "-c" : "",
	     in ? color : "",
	     dest);
  

}

std::ostream& operator<<(std::ostream& os, const gea::AnimAddLink& l) {
  
  inOutLink(true, l.dest, l.color );
  
  return os;
}

std::ostream& operator<<(std::ostream& os, const gea::AnimDelLink& l) {

  inOutLink(false, l.dest );  

  return os;
}

