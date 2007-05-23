#ifndef _POSIXAPIIFACE_H__
#define _POSIXAPIIFACE_H__

#include <gea/ApiIface.h>
#include <stdint.h>

namespace gea {

    class PosixApiIface : public gea::ApiIface 
    {
    public:
	uint32_t ipAddr;
	
	PosixApiIface() : ipAddr(0) {}
	
	virtual ~PosixApiIface() {}
	
	virtual void getCurrentTime(AbsTime *p_now);
	virtual long getCurrentNode();
	
	virtual void createSubEventHandler(EventHandler *eh);
	virtual void destroySubEventHandler(EventHandler *eh);

	virtual void createShadowHandle(Handle *p_handle);
	virtual void destroyShadowHandle(Handle *p_handle);
	
	virtual uint32_t getIpAddr();
	virtual void createSubUdpHandle(UdpHandle *handle, 
					bool receiveMode, 
					const UdpAddress& addr);
	virtual void destroySubUdpHandle(UdpHandle *handle);
	
	virtual void createSubBlocker(Blocker *b);
	virtual void destroySubBlocker(Blocker *b);

	virtual void createSubDepend(DependHandle *dh);
	virtual void destroySubDepend(DependHandle *dh);

    };
    
    void initPosixApiIface();
    
}



#endif //POSIXAPIIFACE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
