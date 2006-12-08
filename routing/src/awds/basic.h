#ifndef _BASIC_H__
#define _BASIC_H__

#include <awds/NodeId.h>

namespace gea {
    
    class Handle;
    
}


struct basic {
    
    NodeId BroadcastId;
    NodeId MyId;
    
    gea::Handle *sendHandle;
    gea::Handle *recvHandle;
    
    virtual void setSendDest(const NodeId& id) = 0;
    virtual void getRecvSrc(NodeId& id) = 0;
    virtual ~basic() = 0;
};


#endif //BASIC_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */