#ifndef _SHADOWHANDLE_H__
#define _SHADOWHANDLE_H__

#include <gea/config.h>


#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif // HAVE_WINSOCK2_H

namespace gea {
    

    /** \brief define mode for GEA Posix handle 
     *  \ingroup PosixAPI
     */
    enum PosixHandleMode {
	PosixModeRead      = 0, ///< event notification for 'is readable'
	PosixModeWrite     = 1, ///< event notification for 'is writeable'
	PosixModeException = 2  ///< event notification for 'has OOB data'
    };
    
    
    class ShadowHandle {
	
    public:
	
	/* --- only for UnixFdHandles and UdpHandles --- */


#ifdef _GNU_H_WINDOWS32_SOCKETS
	typedef SOCKET fd_handle_t;
#else
	typedef int fd_handle_t;
#endif // _GNU_H_WINDOWS32_SOCKETS
	fd_handle_t unixFd;
	enum PosixHandleMode unixMode;
	
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
