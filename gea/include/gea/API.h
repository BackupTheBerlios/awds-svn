#ifndef __GEA_API_H_
#define __GEA_API_H_

#include <gea/EventHandler.h>
#include <gea/Blocker.h>

extern gea::EventHandler GEA;

namespace gea {
  
  gea::EventHandler& geaAPI();
  
}
#endif // __GEA_API_H_
