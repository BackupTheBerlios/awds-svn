
#include <common/scheduler.h>


#include <gea/Time.h>

using namespace gea;

AbsTime AbsTime::now() {
    double time_now = Scheduler::instance().clock();
	return AbsTime( static_cast<AbsTime::StoreType>(time_now * static_cast<double>(AbsTime::offset)) );
}
