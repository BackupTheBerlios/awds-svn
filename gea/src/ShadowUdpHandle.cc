#include <gea/posix/ShadowUdpHandle.h>

#include <iostream>
#include <cassert>
#include <cerrno>

#include <gea/config.h>
#include <gea/compat.h>

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

ShadowUdpAddress::ShadowUdpAddress(int port, const char * ip_addr) :
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




ShadowUdpHandle::ShadowUdpHandle(gea::UdpHandle::Mode mode, const gea::UdpAddress& addr) :
    UnixFdHandle(-1, ( mode == gea::UdpHandle::Send )
		 ? gea::ShadowHandle::Write 
		 : gea::ShadowHandle::Read ),
    addr(addr),
    src(0,UdpAddress::IP_ANY),
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

    int ret = ::bind(fd, 
		     reinterpret_cast<struct sockaddr*>(&(this->src.shadow->addr)),
		     sizeof(struct sockaddr_in) );
    already_bound = true;
    return ret;
}

int ShadowUdpHandle::write(const char *buf, int size) {
    assert(this->shadowHandle->unixMode == gea::ShadowHandle::Write);    
    
    ssize_t ret;
    do {
	ret = ::sendto(this->shadowHandle->unixFd,
		       buf, size,
		       0,
		       reinterpret_cast<const struct sockaddr*>(&(this->addr.shadow->addr)),
		       sizeof(struct sockaddr_in) );
    } while ( (ret == -1) && ( errno == EINTR));
    
    
    return ret;
	
}

int ShadowUdpHandle::read(char *buf, int size) {
    assert ( this->shadowHandle->unixMode == gea::ShadowHandle::Read);

    ssize_t ret;
    recvfrom_lastarg_t src_size /* type defined in gea/compat.h */
	= sizeof(struct sockaddr_in);
    do {
	ret = ::recvfrom(this->shadowHandle->unixFd, 
			 buf, size,
			 0, /* flags */
			 reinterpret_cast<struct sockaddr *>(&(this->src.shadow->addr)),
			 &src_size);
    } while ( (ret == -1) && (errno == EINTR) );
    
    return ret;
}

UdpAddress ShadowUdpHandle::getSrc() const {
    return src;
}

void ShadowUdpHandle::setDest(const UdpAddress &dest_addr) {
    addr = dest_addr;
    
#ifndef WIN32
    if (addr.shadow->broadcast) {
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
