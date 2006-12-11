#include <gea/config.h>

#include <sys/time.h>
#include <time.h>

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif // HAVE_WINDOWS_H
 
#include <gea/Time.h>

gea::AbsTime gea::AbsTime::now() {
    StoreType v; 
	
#if HAVE_WINDOWS_H
    v = GetTickCount() * ( FixNum::offset / 1000L );
#else
    struct timeval tv;
    ::gettimeofday(&tv,0);
    v = StoreType(tv.tv_sec) * FixNum::offset;
    v += StoreType(tv.tv_usec) * ( FixNum::offset / 1000000L );
#endif // HAVE_WINDOWS_H
    return AbsTime(v);
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
