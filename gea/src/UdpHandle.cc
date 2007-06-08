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


#include <cassert>
#include <iostream>

#include <gea/ApiIface.h>
#include <gea/UdpHandle.h>
#include <gea/gea_main.h>


using namespace gea;

DLLEXPORT const uint32_t UdpAddress::IPADDR_ANY = 0;
DLLEXPORT const uint32_t UdpAddress::IPADDR_BROADCAST = 0xffffffffUL;


DLLEXPORT UdpAddress::UdpAddress(u_int32_t ip, u_int16_t port) 
{
    setIP(ip);
    setPort(port);
}


DLLEXPORT UdpAddress::UdpAddress(const UdpAddress& a) :
    ip(a.ip),
    port(a.port),
    isBroadcast(a.isBroadcast)
{ }

DLLEXPORT UdpAddress& UdpAddress::operator=(const UdpAddress& a) {
    ip = a.ip;
    port = a.port;
    isBroadcast = a.isBroadcast;
    return *this;
}



DLLEXPORT unsigned long UdpHandle::getIP() {
    return GEA_apiIface->getIpAddr();
}


#ifndef NO_TYPES_IO

DLLEXPORT std::ostream &operator <<(std::ostream &s,
			  const gea::UdpAddress &a) {
    
    struct in_addr ip_addr;
    
    ip_addr.s_addr = a.ip;
    
    s << inet_ntoa(ip_addr)
      << ":"
      << ntohs(a.port);
    return s;
}

#endif //NO_TYPES_IO



DLLEXPORT UdpHandle::UdpHandle(Mode mode, const UdpAddress& addr) :
    gea::Handle()
{
    GEA_apiIface->createSubUdpHandle(this, mode == Recv, addr);
}

DLLEXPORT UdpHandle::~UdpHandle() {
    GEA_apiIface->destroySubUdpHandle(this);
}

DLLEXPORT int UdpHandle::setSrc(UdpAddress src_addr) {
    return subUdpHandle->setSrc(src_addr);
}

DLLEXPORT void UdpHandle::setDest(UdpAddress dest_addr) {
    subUdpHandle->setDest(dest_addr);
}


DLLEXPORT int UdpHandle::write(const char *buf, int size) {
    return subUdpHandle->write(buf, size);
}

DLLEXPORT int UdpHandle::read(char *buf, int size) {
    return subUdpHandle->read(buf, size);
}

DLLEXPORT UdpAddress UdpHandle::getSrc() const {
    return subUdpHandle->getSrc();
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
