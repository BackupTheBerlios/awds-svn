#include <gea/API.h>
#include <gea/gea_main.h>

namespace gea {
  struct ApiIface;
}

DLLEXPORT gea::EventHandler GEA;
DLLEXPORT gea::ApiIface *GEA_apiIface;


DLLEXPORT gea::EventHandler& gea::geaAPI() {
  return ::GEA;
}

