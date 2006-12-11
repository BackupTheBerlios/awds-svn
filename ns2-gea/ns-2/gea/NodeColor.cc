
#include <gea/API.h>
#include <gea/ShadowEventHandler.h>
#include <gea/NodeColor.h>


std::ostream& operator<<(std::ostream& os, const gea::NodeColor& col) {
  /* do nothing */
  const char *node = GEA.shadow->currentNode->name();
  
  Tcl& tcl = Tcl::instance();
  tcl.evalf("%s color %s", node, col.color);
      
  
  return os;
}

