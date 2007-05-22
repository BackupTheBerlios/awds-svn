
#include <common/scheduler.h>
#include <gea/Time.h>
#include "Ns2ApiIface.h"

using namespace gea;

// AbsTime AbsTime::now() {
//     double time_now = Scheduler::instance().clock();
// 	return AbsTime( static_cast<AbsTime::StoreType>(time_now * static_cast<double>(AbsTime::offset)) );
// }

void Ns2ApiIface::getCurrentTime(AbsTime* p_now) {
    double time_now = Scheduler::instance().clock();
    
    *p_now =  AbsTime( static_cast<AbsTime::StoreType>(time_now * static_cast<double>(AbsTime::offset)) );
    
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
