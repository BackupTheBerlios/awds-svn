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

#include "phy-et.h"
#include "mac-handler.tcc"
#include "hdr-mac-80211.h"
#include "mac-traces.h"

#include "packet.h"
#include <iostream>

#ifndef PHY_ET_TRACE
#define nopePHY_ET_TRACE 1
#endif /* PHY_ET_TRACE */

#ifdef PHY_ET_TRACE
# define TRACE(format, ...) \
	printf ("ET TRACE %d %f " format "\n", selfAddress (), \
                Scheduler::instance ().clock (), ## __VA_ARGS__);
#else /* PHY_ET_TRACE */
# define TRACE(format, ...)
#endif /* PHY_ET_TRACE */


/****************************************************************
 *       The Phy itself.
 ****************************************************************/

PhyEt::PhyEt ()
	: Phy80211 ()
{
	m_endRxHandler = new DynamicHandler<PhyEt> (this, &PhyEt::endRx);
	m_rxPacket = 0;
}

PhyEt::~PhyEt ()
{
	delete m_endRxHandler;
}

void 
PhyEt::startRx (Packet *packet)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	switch (getState ()) {
	case Phy80211::SYNC:
		//		std::cout << "phyet drop" << std::endl;
		TRACE ("drop packet because already in sync");
		assert (m_rxPacket != 0);
		Packet::free (packet);
		break;
	case Phy80211::TX:
		//		std::cout << "phyet drop" << std::endl;
		TRACE ("drop packet because already in tx");
		Packet::free (packet);
		break;
	case Phy80211::SLEEP:
		//		std::cout << "phyet drop" << std::endl;
		Packet::free (packet);
		break;
	case Phy80211::IDLE: {
		assert (m_rxPacket == 0);
		double power = calculateRxPower (packet);
		//		std::cout << "vergleich: " << power << " > " << dBmToW(getRxThreshold()) << std::endl;
		if (power > dBmToW (getRxThreshold ())) {
			// sync to signal
			TRACE ("sync on packet");
			double rxDuration = calculatePacketDuration (0, getPayloadMode (packet),
								     ::getSize (packet));
			notifyRxStart (now (), rxDuration);
			switchToSyncFromIdle (rxDuration);
			m_rxPacket = packet;
			m_endRxHandler->start (rxDuration);
			//			std::cout << "sendpacket" << std::endl;
		} else {
			/* if the energy of the signal is smaller than rxThreshold,
			 * this packet is not synced upon.
			 */
			//			std::cout << "phyet drop packet" << std::endl;
			TRACE ("drop packet");
			Packet::free (packet);
		}
	} break;
	}
}

void
PhyEt::endRx (MacCancelableEvent *ev)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	assert (getState () == Phy80211::SYNC);

	// XXX ???
	setLastRxSNR (calculateRxPower (m_rxPacket));

	HDR_CMN (m_rxPacket)->error () = 0;
	HDR_CMN (m_rxPacket)->direction() = hdr_cmn::UP;
	
	bool receivedOk = true;
	notifyRxEnd (now (), receivedOk);
	switchToIdleFromSync ();
	forwardUp (m_rxPacket);
	m_rxPacket = 0;
}

void 
PhyEt::cancelRx (void)
{
	m_endRxHandler->cancel ();
	Packet::free (m_rxPacket);
	m_rxPacket = 0;
}

