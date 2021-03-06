/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 2004,2005 INRIA
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * In addition, as a special exception, the copyright holders of
 * this module give you permission to combine (via static or
 * dynamic linking) this module with free software programs or
 * libraries that are released under the GNU LGPL and with code
 * included in the standard release of ns-2 under the Apache 2.0
 * license or under otherwise-compatible licenses with advertising
 * requirements (or modified versions of such code, with unchanged
 * license).  You may copy and distribute such a system following the
 * terms of the GNU GPL for this module and the licenses of the
 * other code concerned, provided that you include the source code of
 * that other code when and as the GNU GPL requires distribution of
 * source code.
 *
 * Note that people who make modified versions of this module
 * are not obligated to grant this special exception for their
 * modified versions; it is their choice whether to do so.  The GNU
 * General Public License gives permission to release a modified
 * version without this exception; this exception also makes it
 * possible to release a modified version which carries forward this
 * exception.
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#include "mac-stations.h"
#include "mac-station.h"
#include "arf-mac-stations.h"
#include <iostream>

#include <assert.h>
#include "agent.h"

ArfMacStation::ArfMacStation (MacStations *stations,
			      int min_timer_timeout,
			      int min_success_threshold)
	: MacStation (stations)
{
        m_min_timer_timeout = min_timer_timeout;
        m_min_success_threshold = min_success_threshold;
        m_success_threshold = m_min_success_threshold;
        m_timer_timeout = m_min_timer_timeout;
        m_rate = getMinRate ();

        m_success = 0;
        m_failed = 0;
        m_recovery = false;
        m_retry = 0;
        m_timer = 0;
}
ArfMacStation::~ArfMacStation ()
{}


int 
ArfMacStation::getMaxRate (void)
{
	return getNModes ();
}
int 
ArfMacStation::getMinRate (void)
{
	return 0;
}

bool 
ArfMacStation::needRecoveryFallback (void)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	if (m_retry == 1) {
		return true;
	} else {
		return false;
	}
}
bool 
ArfMacStation::needNormalFallback (void)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	int retry_mod = (m_retry - 1) % 2;
	if (retry_mod == 1) {
		return true;
	} else {
		return false;
	}
}



void 
ArfMacStation::reportRTSFailed (void)
{}
/**
 * It is important to realize that "recovery" mode starts after failure of
 * the first transmission after a rate increase and ends at the first successful
 * transmission. Specifically, recovery mode transcends retransmissions boundaries.
 * Fundamentally, ARF handles each data transmission independently, whether it
 * is the initial transmission of a packet or the retransmission of a packet.
 * The fundamental reason for this is that there is a backoff between each data
 * transmission, be it an initial transmission or a retransmission.
 */
void 
ArfMacStation::reportDataFailed (void)
{
	/*	if (clk_first < 0) {
		clk_first = Scheduler::instance().clock();
	}
	clk_last = Scheduler::instance().clock();*/
        m_timer++;
        m_failed++;
        m_retry++;
        m_success = 0;

        if (m_recovery) {
                assert (m_retry >= 1);
                if (needRecoveryFallback ()) {
                        reportRecoveryFailure ();
                        if (m_rate != getMinRate ()) {
                                m_rate--;
                        }
                }
                m_timer = 0;
        } else {
                assert (m_retry >= 1);
                if (needNormalFallback ()) {
                        reportFailure ();
                        if (m_rate != getMinRate ()) {
                                m_rate--;
                        }
                }
                if (m_retry >= 2) {
                        m_timer = 0;
                }
        }
}
void 
ArfMacStation::reportRxOk (double SNR, int mode)
{}
void ArfMacStation::reportRTSOk (double ctsSNR, int ctsMode)
{}
void ArfMacStation::reportDataOk (double ackSNR, int ackMode)
{
	packet_counter++;
	rate_sum += m_rate;
	//	packet_counter[m_rate]++;
	if (clk_first < 0) {
		clk_first = Scheduler::instance().clock();
	}
	clk_last = Scheduler::instance().clock();
	m_timer++;
        m_success++;
        m_failed = 0;
        m_recovery = false;
        m_retry = 0;
        if ((m_success == getSuccessThreshold () ||
             m_timer == getTimerTimeout ()) &&
            (m_rate < (getMaxRate () - 1))) {
                m_rate++;
                m_timer = 0;
                m_success = 0;
                m_recovery = true;
        }

}
void ArfMacStation::reportFinalRTSFailed (void)
{}
void ArfMacStation::reportFinalDataFailed (void)
{}
int ArfMacStation::getDataMode (int size)
{
	return m_rate;
	/*	if (m_rate > 0) {
		return m_rate-1;
	} else {
		return 0;
		}*/
}
int ArfMacStation::getRTSMode (void)
{
	return 0;
}

void ArfMacStation::reportRecoveryFailure (void)
{}
void ArfMacStation::reportFailure (void)
{}
int ArfMacStation::getMinTimerTimeout (void)
{
        return m_min_timer_timeout;
}
int ArfMacStation::getMinSuccessThreshold (void)
{
        return m_min_success_threshold;
}
int ArfMacStation::getTimerTimeout (void)
{
        return m_timer_timeout;
}
int ArfMacStation::getSuccessThreshold (void)
{
        return m_success_threshold;
}
void ArfMacStation::setTimerTimeout (int timer_timeout)
{
        assert (timer_timeout >= m_min_timer_timeout);
        m_timer_timeout = timer_timeout;
}
void ArfMacStation::setSuccessThreshold (int success_threshold)
{
        assert (success_threshold >= m_min_success_threshold);
        m_success_threshold = success_threshold;
}





ArfMacStations::ArfMacStations ()
	: MacStations ()
{}
ArfMacStations::~ArfMacStations ()
{}
MacStation *
ArfMacStations::createStation (void)
{
	/* XXX: use mac to access user and PHY params. */
	return new ArfMacStation (this, 15, 10);
}

