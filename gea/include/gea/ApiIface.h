#ifndef _APIIFACE_H__
#define _APIIFACE_H__

#include <stdint.h>

namespace gea {
    
    class AbsTime;
    class Handle;
    class UdpHandle;
    class Blocker;
    class DependHandle;
    class UdpAddress;
    class EventHandler;
    
    struct ApiIface {
	
	virtual void getCurrentTime(AbsTime *p_now) = 0;
	virtual long getCurrentNode() = 0;
	
	virtual void createSubEventHandler(EventHandler *eh) = 0;
	virtual void destroySubEventHandler(EventHandler *eh) = 0;
	
	virtual void createShadowHandle(Handle *p_handle) = 0;
	virtual void destroyShadowHandle(Handle *p_handle) = 0;
	
	virtual uint32_t getIpAddr() = 0;
	virtual void createSubUdpHandle(UdpHandle *handle, 
					bool receiveMode, 
					const UdpAddress& addr) = 0;
	virtual void destroySubUdpHandle(UdpHandle *handle) = 0;
	
	virtual void createSubBlocker(Blocker *b) = 0;
	virtual void destroySubBlocker(Blocker *b) = 0;
	
	virtual void createSubDepend(DependHandle *dh) = 0;
	virtual void destroySubDepend(DependHandle *dh) = 0;
	
	virtual ~ApiIface() {}
    };
}

extern gea::ApiIface *GEA_apiIface;


#endif //APIIFACE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
