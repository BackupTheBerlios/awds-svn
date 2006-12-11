#include <gea/Handle.h>
#include <gea/ShadowHandle.h>

gea::Handle::Handle() :
  shadowHandle( new ShadowHandle() )
{
    
}

gea::Handle::~Handle() {
  delete shadowHandle;
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

