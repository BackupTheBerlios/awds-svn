#include <gea/API.h>

namespace gea {
  struct ApiIface;
}

gea::EventHandler GEA;
gea::ApiIface *GEA_apiIface;


gea::EventHandler& gea::geaAPI() {
  return ::GEA;
}

