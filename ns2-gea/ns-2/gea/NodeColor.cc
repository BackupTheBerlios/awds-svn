
#include <gea/API.h>
#include <gea/ShadowEventHandler.h>
#include <gea/NodeColor.h>

using namespace gea;

std::ostream& operator<<(std::ostream& os, const gea::NodeColor& col) {
  /* do nothing */
  ShadowEventHandler *shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler);
  const char *node = shadow->getCurrentNode()->name();
  
  Tcl& tcl = Tcl::instance();
  tcl.evalf("%s color %s", node, col.color);
      
  
  return os;
}

