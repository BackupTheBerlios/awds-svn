#include <gea/API.h>

gea::EventHandler GEA;


gea::EventHandler& gea::geaAPI() {
  return ::GEA;
}


