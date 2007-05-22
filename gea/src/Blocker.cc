
#include <gea/Blocker.h>
#include <gea/ApiIface.h>
//#include <gea/posix/ShadowHandle.h>

using namespace gea;


gea::Blocker::Blocker() :
    Handle()
{
    GEA_apiIface->createSubBlocker(this);
}


gea::Blocker::~Blocker() {
    GEA_apiIface->destroySubBlocker(this);
}


int Blocker::write(const char *buf, int size) {
    return -1;
}

int Blocker::read (char *buf, int size) {
    return -1;
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
