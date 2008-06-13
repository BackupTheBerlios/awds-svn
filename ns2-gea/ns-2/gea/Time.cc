#include <common/scheduler.h>
#include <gea/Time.h>
#include "Ns2ApiIface.h"

using namespace gea;

void Ns2ApiIface::getCurrentTime(AbsTime* p_now) {
    double time_now = Scheduler::instance().clock();
    
#ifdef GEA_SET_TIME_FROM_DOUBLE
    // use this version from GEA 2.4 
    p_now->setSeconds(time_now);
#else
    // pre GEA 2.4
    *p_now =  AbsTime( static_cast<AbsTime::StoreType>(time_now * static_cast<double>(AbsTime::offset)) );
#endif
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
