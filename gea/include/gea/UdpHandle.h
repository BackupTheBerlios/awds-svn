#ifndef _UDPHANDLE_H__
#define _UDPHANDLE_H__

#include <gea/Handle.h>
#include <sys/types.h>

namespace gea {
    
    class UdpAddress {

    public:	
	
	class ShadowUdpAddress * const shadow;
	
	static const char * IP_ANY;
	static const char * IP_BROADCAST;
	
	
	UdpAddress(int port, const char * ip_addr = IP_ANY);
	UdpAddress(u_int32_t ip, u_int16_t port);

	    UdpAddress(const UdpAddress& a);
	UdpAddress& operator=(const UdpAddress& a);
	
	u_int32_t getIP() const;
	
	void      setIP(u_int32_t ip);
	
	u_int16_t getPort() const;
	
	void      setPort(u_int16_t port);
	
	virtual ~UdpAddress();
    };
    
    
    class UdpHandle : public gea::Handle{
	
    public:
	class ShadowUdpHandle * const shadowUdpHandle;
	   	
    public:

	enum Mode {
	    Send = 0,
	    Write = 0,
	    Recv = 1,
	    Read = 1

	};

	UdpHandle(Mode mode, const UdpAddress& addr);

	static unsigned long getIP();

	virtual ~UdpHandle();
    
	int setSrc(UdpAddress src_addr);
	void setDest(UdpAddress dest_addr);
	
	UdpAddress getSrc() const;
	UdpAddress getDest() const;
	
	virtual int write(const char *buf, int size);
	virtual int read (char *buf, int size);
	
    };
    
}

#ifndef NO_TYPES_IO
#include <iostream>
    
std::ostream& operator <<(std::ostream &s, const gea::UdpAddress &a); 
#endif // NO_TYPES_IO



#endif //UDPHANDLE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
