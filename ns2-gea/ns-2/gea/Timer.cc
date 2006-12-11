

#include "Timer.h"

using namespace gea;


const char * Blocker::TypeName="Blocker";

const char * Blocker::getType() const {
    return Blocker::TypeName;
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
