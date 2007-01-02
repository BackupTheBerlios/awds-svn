#ifndef _COMPAT_H__
#define _COMPAT_H__

#include <gea/config.h>
#include <sys/types.h>

#ifdef WIN32
  typedef int    recvfrom_lastarg_t;
#else

# include <sys/socket.h>
  typedef socklen_t recvfrom_lastarg_t;
#endif


bool initSocketLayer();

bool closeSocketLayer();




#endif //COMPAT_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

