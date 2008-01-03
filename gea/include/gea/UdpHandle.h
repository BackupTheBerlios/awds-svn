#ifndef _UDPHANDLE_H__
#define _UDPHANDLE_H__

#include <gea/Handle.h>
// #include <sys/types.h>
#include <stdint.h>

namespace gea {

    /** \brief class for representing a UDP address
     *  \ingroup GEA_API
     */
    class UdpAddress {

    public:

	uint32_t ip;    ///< the IP address
	uint16_t port;  ///< the port number
	bool     isBroadcast;

	static const uint32_t IPADDR_ANY;
	static const uint32_t IPADDR_BROADCAST;

	/** create address from nummeric IP address and port number*/
	UdpAddress(u_int32_t ip, u_int16_t port);


	/** copy contructor */
	UdpAddress(const UdpAddress& a);
	UdpAddress& operator=(const UdpAddress& a);

	uint32_t getIP() const { return this->ip; }
	void     setIP(uint32_t ip) {this->ip = ip;  isBroadcast = (ip == IPADDR_BROADCAST); }
	uint16_t getPort() const { return this->port; }
	void     setPort(uint16_t port) {this->port = port; }

    };

    class SubUdpHandle {
    public:
	class UdpHandle *master;

	virtual int setSrc(const UdpAddress& src_addr) = 0 ;
	virtual void setDest(const UdpAddress& dest_addr) = 0;

	virtual UdpAddress getSrc() const = 0;
	virtual UdpAddress getDest() const = 0;

	virtual int write(const char *buf, int size) = 0;
	virtual int read (char *buf, int size) = 0;
	virtual ~SubUdpHandle() {};
    };


    /** \brief class for representing a UDP socket.
     *  \ingroup GEA_API
     */
    class UdpHandle : public gea::Handle{

    public:
	class SubUdpHandle * subUdpHandle;

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
