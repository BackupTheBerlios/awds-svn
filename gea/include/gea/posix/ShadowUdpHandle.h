#ifndef _SHADOWUDPHANDLE_H__
#define _SHADOWUDPHANDLE_H__

#include <gea/config.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif // HAVE_SYS_SOCKET_H

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif // HAVE_NETINET_IN_H

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif // HAVE_ARPA_INET_H

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif // HAVE_WINSOCK2_H



#include <gea/posix/UnixFdHandle.h>
#include <gea/UdpHandle.h>

namespace gea {
    /*
    class ShadowUdpAddress {

    public:

	bool broadcast;

	struct sockaddr_in addr;

	ShadowUdpAddress(int port, const char * ip_addr = gea::UdpAddress::IP_ANY);
	ShadowUdpAddress(const ShadowUdpAddress& a);
	ShadowUdpAddress& operator=(const ShadowUdpAddress &a);
    };
    */


    class ShadowUdpHandle : public SubUdpHandle, public UnixFdHandle {

    protected:
	UdpAddress addr;
	UdpAddress src; // only needed for reading
	bool already_bound;
    public:
	ShadowUdpHandle(gea::UdpHandle::Mode mode, const gea::UdpAddress& addr);

	virtual ~ShadowUdpHandle();

	virtual int setSrc(const UdpAddress& src_addr);
	virtual void setDest(const UdpAddress &dest_addr);
	virtual int write(const char *buf, int size);

	virtual int read (char *buf, int size);
	virtual UdpAddress getSrc() const;
	virtual UdpAddress getDest() const;
    };

}

#endif //SHADOWUDPHANDLE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
