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

#include <gea/UdpHandle.h>
#include <gea/posix/ShadowUdpHandle.h>
#include <gea/posix/ShadowHandle.h>


using namespace gea;

const char * UdpAddress::IP_ANY = "ip_any";
const char * UdpAddress::IP_BROADCAST = "ip_broadcast";

UdpAddress::UdpAddress(int port, const char * ip_addr) 
    :shadow(new ShadowUdpAddress(port, ip_addr) )
{}

UdpAddress::UdpAddress(u_int32_t ip, u_int16_t port) 
    :shadow(new ShadowUdpAddress(port, IP_ANY) )
{
    setIP(ip);
    setPort(port);
}


UdpAddress::UdpAddress(const UdpAddress& a) :
    shadow( new ShadowUdpAddress( *(a.shadow)) )
{ }

UdpAddress& UdpAddress::operator=(const UdpAddress& a) {
    *(this->shadow) = *(a.shadow);
    return *this;
}


u_int32_t UdpAddress::getIP() const {
    return shadow->addr.sin_addr.s_addr; 
}

void UdpAddress::setIP(u_int32_t ip) { 
    shadow->addr.sin_addr.s_addr = ip; 
}

u_int16_t UdpAddress::getPort() const {
    return ntohs(shadow->addr.sin_port); 
} 

void UdpAddress::setPort(u_int16_t port) { 
    shadow->addr.sin_port = htons(port); 
}



void guess_iface(char *ret);

unsigned long UdpHandle::getIP() {
    //  assert(this->shadowHandle->isUdpHandle());
    char buf[20];
    guess_iface(buf);
    
    return (unsigned long)inet_addr(buf);
    
}

UdpAddress::~UdpAddress() 
{
    delete shadow;
}



#ifndef NO_TYPES_IO

std::ostream &operator <<(std::ostream &s,
			  const gea::UdpAddress &a) {
    
    s << inet_ntoa(a.shadow->addr.sin_addr)
      << ":"
      << ntohs(a.shadow->addr.sin_port);
    return s;
}

#endif //NO_TYPES_IO



UdpHandle::UdpHandle(Mode mode, const UdpAddress& addr) :
    gea::Handle(),
    shadowUdpHandle(new ShadowUdpHandle(mode , addr))
{
    this->shadowHandle->handleType = ShadowHandle::TypeUdpHandle;
}

UdpHandle::~UdpHandle() {
    delete shadowUdpHandle;
}

int UdpHandle::setSrc(UdpAddress src_addr) {
    return shadowUdpHandle->setSrc(src_addr);
}

void UdpHandle::setDest(UdpAddress dest_addr) {
    shadowUdpHandle->setDest(dest_addr);
}


int UdpHandle::write(const char *buf, int size) {
    return shadowUdpHandle->write(buf, size);
}

int UdpHandle::read(char *buf, int size) {
    return shadowUdpHandle->read(buf, size);
}

UdpAddress UdpHandle::getSrc() const {
    return shadowUdpHandle->getSrc();
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
