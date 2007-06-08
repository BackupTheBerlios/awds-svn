
#include <gea/Handle.h>
// #include <gea/posix/ShadowHandle.h>
#include <gea/ApiIface.h>
#include <gea/gea_main.h>

#include <cassert>

DLLEXPORT gea::Handle::Handle() 
{
  GEA_apiIface->createShadowHandle(this);
}

DLLEXPORT gea::Handle::~Handle() {
  GEA_apiIface->destroyShadowHandle(this);
}

