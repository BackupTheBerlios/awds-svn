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
//#include <gea/posix/ShadowUdpHandle.h>
//#include <gea/posix/ShadowHandle.h>


using namespace gea;

// const char * UdpAddress::IP_ANY = "ip_any";
// const char * UdpAddress::IP_BROADCAST = "ip_broadcast";

const uint32_t UdpAddress::IPADDR_ANY = 0;
const uint32_t UdpAddress::IPADDR_BROADCAST = 0xffffffffUL;

// UdpAddress::UdpAddress(int port, const char * ip_addr) 
//     :shadow(new ShadowUdpAddress(port, ip_addr) )
// {}

UdpAddress::UdpAddress(u_int32_t ip, u_int16_t port) 
{
    setIP(ip);
    setPort(port);
}


UdpAddress::UdpAddress(const UdpAddress& a) :
    ip(a.ip),
    port(a.port),
    isBroadcast(a.isBroadcast)
{ }

UdpAddress& UdpAddress::operator=(const UdpAddress& a) {
    ip = a.ip;
    port = a.port;
    isBroadcast = a.isBroadcast;
    return *this;
}


// u_int32_t UdpAddress::getIP() const {
//     return shadow->addr.sin_addr.s_addr; 
// }

// void UdpAddress::setIP(u_int32_t ip) { 
//     shadow->addr.sin_addr.s_addr = ip; 
// }

// u_int16_t UdpAddress::getPort() const {
//     return ntohs(shadow->addr.sin_port); 
// } 

// void UdpAddress::setPort(u_int16_t port) { 
//     shadow->addr.sin_port = htons(port); 
// }


unsigned long UdpHandle::getIP() {
    return GEA_apiIface->getIpAddr();
}


#ifndef NO_TYPES_IO

std::ostream &operator <<(std::ostream &s,
			  const gea::UdpAddress &a) {
    
    struct in_addr ip_addr;
    
    ip_addr.s_addr = a.ip;
    
    s << inet_ntoa(ip_addr)
      << ":"
      << ntohs(a.port);
    return s;
}

#endif //NO_TYPES_IO



UdpHandle::UdpHandle(Mode mode, const UdpAddress& addr) :
    gea::Handle()
{
    GEA_apiIface->createSubUdpHandle(this, mode == Recv, addr);
}

UdpHandle::~UdpHandle() {
    GEA_apiIface->destroySubUdpHandle(this);
}

int UdpHandle::setSrc(UdpAddress src_addr) {
    return subUdpHandle->setSrc(src_addr);
}

void UdpHandle::setDest(UdpAddress dest_addr) {
    subUdpHandle->setDest(dest_addr);
}


int UdpHandle::write(const char *buf, int size) {
    return subUdpHandle->write(buf, size);
}

int UdpHandle::read(char *buf, int size) {
    return subUdpHandle->read(buf, size);
}

UdpAddress UdpHandle::getSrc() const {
    return subUdpHandle->getSrc();
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
