/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 2005 INRIA
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
#ifndef ARF_MAC_STATIONS_H
#define ARF_MAC_STATIONS_H

#include "mac-stations.h"
#include "mac-station.h"
#include <map>

class ArfMacStations : public MacStations {
public:
	ArfMacStations ();
	virtual ~ArfMacStations ();
private:
	virtual class MacStation *createStation (void);
};


class ArfMacStation : public MacStation {
public:
	ArfMacStation (MacStations *stations,
		       int min_timer_timeout,
		       int min_success_threshold);
	virtual ~ArfMacStation ();

	virtual void reportRxOk (double SNR, int mode);

	virtual void reportRTSFailed (void);
	virtual void reportDataFailed (void);
	virtual void reportRTSOk (double ctsSNR, int ctsMode);
	virtual void reportDataOk (double ackSNR, int ackMode);
	virtual void reportFinalRTSFailed (void);
	virtual void reportFinalDataFailed (void);
	virtual int getDataMode (int size);
	virtual int getRTSMode (void);

private:
	int m_timer;
	int m_success;
	int m_failed;
	bool m_recovery;
	int m_retry;
	
	int m_timer_timeout;
	int m_success_threshold;

	int m_rate;
	
	int m_min_timer_timeout;
	int m_min_success_threshold;
	
private:
	virtual void reportRecoveryFailure (void);
	virtual void reportFailure (void);

	int getMaxRate (void);
	int getMinRate (void);

	bool needRecoveryFallback (void);
	bool needNormalFallback (void);
	
protected:
	int getMinTimerTimeout (void);
	int getMinSuccessThreshold (void);
	
	int getTimerTimeout (void);
	int getSuccessThreshold (void);
	
	void setTimerTimeout (int timer_timeout);
	void setSuccessThreshold (int success_threshold);
};



#endif /* ARF_MAC_STATIONS_H */
