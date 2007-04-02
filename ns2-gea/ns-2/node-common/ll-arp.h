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

#ifndef LL_ARP_H
#define LL_ARP_H

#include "sgi-hashmap.h"
#include <list>

class NetInterface;
class Packet;
class ArpCacheEntry;
class MacHigh;

class LLArp {
public:
	LLArp ();
	~LLArp ();

	void setInterface (NetInterface *interface);
	void setMac (MacHigh *mac);
	void setTimeouts (double aliveTimeout, 
			  double deadTimeout,
			  double waitReplyTimeout);

	/* called by the IP layer. */
	void sendDown (Packet *packet);

	/* called by the MAC */
	void sendUp (Packet *packet);

	double getAliveTimeout (void);
	double getDeadTimeout (void);
	double getWaitReplyTimeout (void);

private:
	void ensureQueueIsFlushed (void);
	void dropDeadPacket (Packet *packet);
	void sendArpReply (int macAddress, int32_t ipAddress);
	void sendArpRequest (int32_t ipAddress);

	typedef Sgi::hash_map<int32_t, ArpCacheEntry *> ArpCache;
	typedef Sgi::hash_map<int32_t, ArpCacheEntry *>::iterator ArpCacheI;
	typedef std::list<Packet *> LLPacketQueue;
	typedef std::list<Packet *>::iterator LLPacketQueueI;

	NetInterface *m_interface;
	MacHigh *m_mac;
	ArpCache m_arpCache;
	LLPacketQueue m_queue;
	double m_aliveTimeout;
	double m_deadTimeout;
	double m_waitReplyTimeout;
};


#endif /* LL_ARP_H */
