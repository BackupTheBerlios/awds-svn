#include <gea/posix/ShadowUdpHandle.h>

#include <iostream>
#include <cassert>
#include <cerrno>

#include <gea/config.h>
#include <gea/compat.h>
#include <gea/posix/PosixApiIface.h>

#include <unistd.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif // HAVE_SYS_SOCKET_H

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif // HAVE_SYS_IOCTL_H 

#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif // HAVE_NET_IF_H

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif // HAVE_NETINET_IN_H

using namespace gea;

void guess_iface(char *ret);

uint32_t gea::PosixApiIface::getIpAddr() {
    
    if (this->ipAddr) return this->ipAddr;
    char buf[20];
    guess_iface(buf);
    
    return (uint32_t)inet_addr(buf);
    
}



#if 0
void x_ShadowUdpAddress(int port, const char * ip_addr) :
    broadcast(false)
{
    
    
    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(port);
    if (ip_addr == gea::UdpAddress::IP_ANY) { // yes this is a pointer comparision !
	this->addr.sin_addr.s_addr = INADDR_ANY;
    } else if (ip_addr == gea::UdpAddress::IP_BROADCAST) {
	this->addr.sin_addr.s_addr = INADDR_BROADCAST;
	this->broadcast = true;
    }
    else{
	//	this->addr.sin_addr.s_addr = inet_addr(ip_addr);
	// 	int res = inet_aton(ip_addr, &(this->addr.sin_addr) );
	this->addr.sin_addr.s_addr = inet_addr(ip_addr);
	//	assert ((res != 0));

	if ((addr.sin_addr.s_addr & 0xff000000)==0xff000000) this->broadcast = true;
    }
};


ShadowUdpAddress::ShadowUdpAddress(const ShadowUdpAddress& a) {
    memcpy(&(addr), &(a.addr), sizeof(struct sockaddr_in) ); 
    broadcast = a.broadcast;
    assert( addr.sin_family == AF_INET );
};

ShadowUdpAddress& ShadowUdpAddress::operator=(const ShadowUdpAddress &a) {
    memcpy(&(addr), &(a.addr), sizeof(struct sockaddr_in) ); 
    broadcast = a.broadcast;
    assert( addr.sin_family == AF_INET );
    return *this;
}

#endif

void PosixApiIface::createSubUdpHandle(UdpHandle *handle, 
				       bool receiveMode,
				       const UdpAddress& addr) 
{
    ShadowUdpHandle *h = new ShadowUdpHandle(receiveMode ? UdpHandle::Recv : UdpHandle::Send,
					     addr);
    
    handle->subUdpHandle = h;
    h->master = handle;
    handle->shadowHandle->handleType = ShadowHandle::TypeUdpHandle;
}

void PosixApiIface::destroySubUdpHandle(UdpHandle *handle) 
{
    assert(handle->subUdpHandle);
    delete handle->subUdpHandle;
    handle->subUdpHandle = 0;
}



ShadowUdpHandle::ShadowUdpHandle(gea::UdpHandle::Mode mode, const gea::UdpAddress& addr) :
    UnixFdHandle(-1, ( mode == gea::UdpHandle::Send )
		 ? gea::ShadowHandle::Write 
		 : gea::ShadowHandle::Read ),
    addr(addr),
    src(UdpAddress::IPADDR_ANY, 0),
    already_bound(false)
{
    int udp_socket = socket(PF_INET, SOCK_DGRAM, 0);    
    assert( udp_socket >= 0);
    this->shadowHandle->unixFd = udp_socket;    
    
    this->setDest(addr);
    
    if (udp_socket == -1) {
	this->status = Error;
	return;
    }
    
    if (this->shadowHandle->unixMode == gea::ShadowHandle::Read)
	this->setSrc(addr);

}

ShadowUdpHandle::~ShadowUdpHandle() {
    int  fd = this->shadowHandle->unixFd;
    if (fd > 0) {
	close(fd);
    }
    fd = -1;
}

int ShadowUdpHandle::setSrc(const UdpAddress &src_addr) {

    int fd = this->shadowHandle->unixFd;

if (already_bound) {
	int new_socket = socket(PF_INET, SOCK_DGRAM, 0);    
	dup2(new_socket, fd);
	close(new_socket);
    }

    this->src = src_addr;
    
#ifndef WIN32
    int opt = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));
#endif
    
    struct sockaddr_in x_addr;
    x_addr.sin_family      = AF_INET;
    x_addr.sin_addr.s_addr = htonl(src_addr.ip);
    x_addr.sin_port        = htons(src_addr.port);
    
    int ret = ::bind(fd, 
		     (const struct sockaddr *)&x_addr,
		     sizeof(struct sockaddr_in) );
    already_bound = true;
    return ret;
}

int ShadowUdpHandle::write(const char *buf, int size) {
    assert(this->shadowHandle->unixMode == gea::ShadowHandle::Write);    
    
    ssize_t ret;

    struct sockaddr_in x_addr;
    x_addr.sin_family      = AF_INET;
    x_addr.sin_addr.s_addr = htonl(addr.ip);
    x_addr.sin_port        = htons(addr.port);

    do {
	ret = ::sendto(this->shadowHandle->unixFd,
		       buf, size,
		       0,
		       (const struct sockaddr *)&x_addr,
		       sizeof(struct sockaddr_in) );
    } while ( (ret == -1) && ( errno == EINTR));
    
    
    return ret;
	
}

int ShadowUdpHandle::read(char *buf, int size) {
    assert ( this->shadowHandle->unixMode == gea::ShadowHandle::Read);

    ssize_t ret;
    recvfrom_lastarg_t src_size /* type defined in gea/compat.h */
	= sizeof(struct sockaddr_in);

    struct sockaddr_in x_addr;
    
    do {
	ret = ::recvfrom(this->shadowHandle->unixFd, 
			 buf, size,
			 0, /* flags */
			 (struct sockaddr *)&x_addr,
			 &src_size);
    } while ( (ret == -1) && (errno == EINTR) );
    
    src.setIP(   ntohl(x_addr.sin_addr.s_addr) );
    src.setPort( ntohs(x_addr.sin_port) );
    return ret;
}

UdpAddress ShadowUdpHandle::getSrc() const {
    return src;
}

void ShadowUdpHandle::setDest(const UdpAddress &dest_addr) {
    addr = dest_addr;
    
#ifndef WIN32
    if (addr.isBroadcast) {
	int opt = 1;
	::setsockopt(this->shadowHandle->unixFd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof (opt));
    }
#endif // WIN32     
}

UdpAddress ShadowUdpHandle::getDest() const {
    return addr;
}






/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
