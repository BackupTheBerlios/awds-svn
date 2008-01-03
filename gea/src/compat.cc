
#include <gea/compat.h>

#ifdef WIN32
#include <winsock2.h>
#endif




bool initSocketLayer() {
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
	/* Tell the user that we could not find a usable */
	/* WinSock DLL.                                  */
	return false;
    }


#endif
    return true;
}


bool closeSocketLayer() {
#ifdef WIN32
  WSACleanup( );
#endif
  return true;
}
