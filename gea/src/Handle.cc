
#include <gea/Handle.h>
// #include <gea/posix/ShadowHandle.h>
#include <gea/ApiIface.h>

#include <cassert>

gea::Handle::Handle() 
{
  GEA_apiIface->createShadowHandle(this);
}

gea::Handle::~Handle() {
  GEA_apiIface->destroyShadowHandle(this);
}

