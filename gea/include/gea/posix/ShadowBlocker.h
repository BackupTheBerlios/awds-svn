#ifndef _TIMER_H__
#define _TIMER_H__

#include "Events.h"

namespace gea {

class Blocker : public Handle {
    
public: 
    static const char * TypeName;
    
    virtual const char *getType() const;
    virtual int write(const char *buf, int size);
    virtual int read (char *buf, int size);
};

};

#endif //TIMER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
