#include <gea/config.h>

#include <sys/time.h>
#include <time.h>

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif // HAVE_WINDOWS_H

#include <gea/Time.h>
#include <gea/posix/PosixApiIface.h>


void gea::PosixApiIface::getCurrentTime(gea::AbsTime *p_now) {

#if HAVE_WINDOWS_H
    p_now->v = GetTickCount() * ( FixNum::offset / 1000L );
#else
    struct timeval tv;
    ::gettimeofday(&tv,0);
    p_now->v = AbsTime::StoreType(tv.tv_sec) * FixNum::offset;
    p_now->v += AbsTime::StoreType(tv.tv_usec) * ( FixNum::offset / 1000000L );
#endif // HAVE_WINDOWS_H

}





/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
