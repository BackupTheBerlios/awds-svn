
#include <gea/Handle.h>
#include <gea/posix/ShadowHandle.h>

#include <cassert>

gea::Handle::Handle() :
  shadowHandle(new gea::ShadowHandle() )
{
  assert(this->shadowHandle != 0);
}

gea::Handle::~Handle() {
  assert(this->shadowHandle != 0);
  delete this->shadowHandle;
}
