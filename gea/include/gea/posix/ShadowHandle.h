#ifndef _SHADOWHANDLE_H__
#define _SHADOWHANDLE_H__

#include <gea/config.h>


#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif // HAVE_WINSOCK2_H

namespace gea {

    class ShadowHandle {
	
    public:
	
	/* --- only for UnixFdHandles and UdpHandles --- */

	enum UnixMode {
	    Read      = 0, // must have this value for array trick
	    Write     = 1,
	    Exception = 2
	};
#ifdef _GNU_H_WINDOWS32_SOCKETS
	typedef SOCKET fd_handle_t;
#else
	typedef int fd_handle_t;
#endif // _GNU_H_WINDOWS32_SOCKETS
	fd_handle_t unixFd;
	enum UnixMode unixMode;
	
	enum HandleType {
	    TypeUnixFdHandle = 14,
	    TypeBlocker,
	    TypeDepend,
	    TypeUdpHandle
	};
	
	HandleType handleType;
	
	bool isUnixFdHandle() const { return this->handleType == TypeUnixFdHandle; }
	bool isUdpHandle() const    { return this->handleType == TypeUdpHandle; }
	bool isBlocker() const      { return this->handleType == TypeBlocker; }
	bool isDepend() const       { return this->handleType == TypeDepend; }
    
    };
    
}

#endif //SHADOWHANDLE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
