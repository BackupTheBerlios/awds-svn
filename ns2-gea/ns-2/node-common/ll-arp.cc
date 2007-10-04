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

/**
 * This file implements ARP-type address resolution of IP addresses to
 * Mac addresses.
 * See rfc 826
 */

#include "packet.h"
#include "scheduler.h"
#include "ip.h"

#include "hdr-marp.h"
#include "ll-arp.h"
#include "net-interface-80211.h"
#include "common.h"
#include "mac-high.h"
#include <iostream>

#define nopeLLARP_TRACE 1

#ifdef LLARP_TRACE
# define TRACE(format, ...) \
	printf ("LLARP TRACE %d %f " format "\n", m_interface->getMacAddress (), \
                Scheduler::instance ().clock (), ## __VA_ARGS__);
#else /* LLARP_TRACE */
# define TRACE(format, ...)
#endif /* LLARP_TRACE */



class ArpCacheEntry {
public:
	enum ArpCacheEntryState_e {
		ALIVE,
		WAIT_REPLY,
		DEAD
	};

	ArpCacheEntry (LLArp *arp)
		: m_arp (arp),
		  m_state (WAIT_REPLY)
	{updateSeen ();}


	ArpCacheEntryState_e getState (void) {
		return m_state;
	}

	void markDead (void) {
		m_state = DEAD;
		updateSeen ();
	}

	void markAlive (int macAddress) {
		m_macAddress = macAddress;
		m_state = ALIVE;
		updateSeen ();
	}

	void markWaitReply (void) {
		m_state = WAIT_REPLY;
		updateSeen ();
	}

	/* the mac address is valid only if state is ALIVE */
	int getMacAddress (void) {
		return m_macAddress;
	}
	bool isExpired (void) {
		double timeout;
		switch (m_state) {
		case ArpCacheEntry::WAIT_REPLY:
			timeout = m_arp->getWaitReplyTimeout ();
			break;
		case ArpCacheEntry::DEAD:
			timeout = m_arp->getDeadTimeout ();
			break;
		case ArpCacheEntry::ALIVE:
			timeout = m_arp->getAliveTimeout ();
			break;
		}
		double elapsedSinceLastEvent = now () - m_lastSeenTime;
		if (elapsedSinceLastEvent >= timeout) {
			return true;
		} else {
			return false;
		}
	}
	void updateSeen (void) {
		m_lastSeenTime = now();
	}
private:
	double now (void) {
		return Scheduler::instance ().clock ();
	}
	LLArp *m_arp;
	ArpCacheEntryState_e m_state;
	int m_macAddress;
	double m_lastSeenTime;
};


LLArp::LLArp ()
	: m_aliveTimeout (1200),
	  m_deadTimeout (100),
	  m_waitReplyTimeout (1)
{}

LLArp::~LLArp ()
{
	LLPacketQueueI i = m_queue.begin ();
	while (i != m_queue.end ()) {
		Packet::free (*i);
		i = m_queue.erase (i);
	}
}

void
LLArp::setInterface (NetInterface *interface)
{
	m_interface = interface;
}
void
LLArp::setMac (MacHigh *mac)
{
	m_mac = mac;
}

void 
LLArp::setTimeouts (double aliveTimeout, 
		    double deadTimeout,
		    double waitReplyTimeout)
{
	m_aliveTimeout = aliveTimeout;
	m_deadTimeout = deadTimeout;
	m_waitReplyTimeout = waitReplyTimeout;
}

double
LLArp::getAliveTimeout (void)
{
	return m_aliveTimeout;
}
double 
LLArp::getDeadTimeout (void)
{
	return m_deadTimeout;
}
double 
LLArp::getWaitReplyTimeout (void)
{
	return m_waitReplyTimeout;
}



void
LLArp::sendArpRequest (int32_t ipAddress)
{
	Packet *packet = Packet::alloc ();
	HDR_CMN (packet)->ptype () = PT_ARP;
	HDR_CMN (packet)->size () = MARP_HDR_LEN;
	HDR_MARP (packet)->arp_op = MARPOP_REQUEST;
	HDR_MARP (packet)->arp_sha = m_interface->getMacAddress ();
	HDR_MARP (packet)->arp_spa = m_interface->getIpAddress ();
	HDR_MARP (packet)->arp_tpa = ipAddress;
	m_mac->enqueueFromLL (packet, MAC_BROADCAST);
}

void
LLArp::sendArpReply (int macAddress, int32_t ipAddress)
{
	Packet *packet = Packet::alloc ();
	HDR_CMN (packet)->ptype () = PT_ARP;
	HDR_CMN (packet)->size () = MARP_HDR_LEN;
	HDR_MARP (packet)->arp_op = MARPOP_REPLY;
	HDR_MARP (packet)->arp_sha = m_interface->getMacAddress ();
	HDR_MARP (packet)->arp_spa = m_interface->getIpAddress ();	
	HDR_MARP (packet)->arp_tha = macAddress;
	HDR_MARP (packet)->arp_tpa = ipAddress;
	m_mac->enqueueFromLL (packet, macAddress);
}

void
LLArp::dropDeadPacket (Packet *packet)
{
	Packet::free (packet);
}

void
LLArp::ensureQueueIsFlushed (void)
{
	LLPacketQueueI i = m_queue.begin ();
	while (i != m_queue.end ()) {
		Packet *packet = *i;
		int32_t ipDest = HDR_IP (packet)->daddr ();
		// <begin> jenz::inria
		if (ipDest == IP_BROADCAST) {
			i = m_queue.erase(i);
			m_mac->enqueueFromLL(packet,MAC_BROADCAST);
			return;
		}	
		// <end> jenz::inria
		assert (m_arpCache.find (ipDest) != m_arpCache.end ());		
		ArpCacheEntry *entry = m_arpCache[ipDest];
		assert (entry != 0);
		switch (entry->getState ()) {
		case ArpCacheEntry::DEAD:
			i = m_queue.erase (i);
			dropDeadPacket (packet);
			break;
		case ArpCacheEntry::WAIT_REPLY:
			i++;
			break;
		case ArpCacheEntry::ALIVE:
			i = m_queue.erase (i);
			m_mac->enqueueFromLL (packet, entry->getMacAddress ());
			break;
		}
	}
}

void 
LLArp::sendDown (Packet *packet)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	int32_t ipDest = HDR_IP (packet)->daddr ();
	ArpCacheEntry *entry;
	// <begin> jenz::inria
	if (ipDest == (int32_t)IP_BROADCAST) {
		m_mac->enqueueFromLL (packet, (int32_t)IP_BROADCAST);
	} else {
		// <end> jenz::inria
		if (m_arpCache.find (ipDest) != m_arpCache.end ()) {			
			entry = m_arpCache[ipDest];			
			assert (entry != 0);
			if (entry->isExpired ()) {
				switch (entry->getState ()) {
				case ArpCacheEntry::WAIT_REPLY:
					TRACE ("wait reply for %d expired -- drop", ipDest);
					entry->markDead ();
					dropDeadPacket (packet);
					break;
				case ArpCacheEntry::DEAD:
					TRACE ("dead entry for %d expired -- send arp request", ipDest);
					entry->markWaitReply ();
					sendArpRequest (ipDest);
					m_queue.push_back (packet);
					break;
				case ArpCacheEntry::ALIVE:
					TRACE ("alive entry for %d expired -- send arp request", ipDest);
					entry->markWaitReply ();
					sendArpRequest (ipDest);
					m_queue.push_back (packet);
					break;
				}
			} else {
				switch (entry->getState ()) {
				case ArpCacheEntry::WAIT_REPLY:
					TRACE ("wait reply for %d valid -- queue", ipDest);
					m_queue.push_back (packet);
					break;
				case ArpCacheEntry::DEAD:
					TRACE ("dead entry for %d valid -- drop", ipDest);
					dropDeadPacket (packet);
					break;
				case ArpCacheEntry::ALIVE:
					TRACE ("alive entry for %d valid -- send", ipDest);
					m_mac->enqueueFromLL (packet, entry->getMacAddress ());
					break;
				}
			}
		} else {
			// This is our first attempt to transmit data to this destination.
			TRACE ("no entry for %d -- send arp request", ipDest);
			entry = new ArpCacheEntry (this);
			sendArpRequest (ipDest); 
			m_queue.push_back (packet);
			m_arpCache[ipDest] = entry;
		}
		// <begin> jenz::inria
	}
	// <end> jenz::inria
	ensureQueueIsFlushed (); 
}

void 
LLArp::sendUp (Packet *packet)
{
	if (HDR_CMN (packet)->ptype () == PT_ARP) {
		int32_t ipAddress = HDR_MARP (packet)->arp_spa;
		int macAddress = HDR_MARP (packet)->arp_sha;
		macAddress = ipAddress; // hack, eigentlich sollten die daten richtig ankommen, gehen aber anscheinend irgendwo kaputt
		ArpCacheEntry *entry;
		if (ipAddress != -1) {
			if (m_arpCache.find (ipAddress) != m_arpCache.end ()) {
				entry = m_arpCache[ipAddress];
				TRACE ("update entry %d", ipAddress);
				entry->markAlive (macAddress);
			} else {
				TRACE ("add new entry %d --> %d", ipAddress, macAddress);
				entry = new ArpCacheEntry (this);
				entry->markAlive (macAddress);
				m_arpCache[ipAddress] = entry;
			}
		}
		if (HDR_MARP (packet)->arp_op == MARPOP_REQUEST &&
		    HDR_MARP (packet)->arp_tpa == m_interface->getIpAddress ()) {
			TRACE ("got arp request for %d -- send reply", 
			       ipAddress);
			sendArpReply (HDR_MARP (packet)->arp_sha, HDR_MARP (packet)->arp_spa);
		} else {
			if (HDR_MARP (packet)->arp_op == MARPOP_REPLY &&
			    HDR_MARP (packet)->arp_tha == m_interface->getMacAddress ()) {
				TRACE ("got arp reply for %d", ipAddress);
			}
		}
		ensureQueueIsFlushed ();
	} else {
		m_interface->sendUpToNode (packet);
	}
}
