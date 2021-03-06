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

#include <list>
#include <math.h>

#include "packet.h"
#include "scheduler.h"

#include "free-space-broadcast-channel.h"
#include "net-node.h"
#include "net-interface.h"
#include <iostream>

class FreeSpaceChannelEvent : public Event {
public:
	FreeSpaceChannelEvent (NetInterface *interface, Packet *packet)
		: m_interface (interface), m_packet (packet) {}
	void run (void) {
		m_interface->sendUp (m_packet);
	}
private:
	NetInterface *m_interface;
	Packet *m_packet;
};


class FreeSpaceChannelHandler : public Handler {
public:
	FreeSpaceChannelHandler () {}
	virtual ~FreeSpaceChannelHandler () {}
	virtual void handle(Event* event) {
		FreeSpaceChannelEvent *ev = static_cast <FreeSpaceChannelEvent *> (event);
		ev->run ();
		delete ev;
	}
};



FreeSpaceBroadcastChannel::FreeSpaceBroadcastChannel ()
	: m_handler (new FreeSpaceChannelHandler ())
{}
FreeSpaceBroadcastChannel::~FreeSpaceBroadcastChannel ()
{
	delete m_handler;
}

#define SPEED_OF_LIGHT (300000000)

double
FreeSpaceBroadcastChannel::calc_delay (NodePosition *source, NodePosition *dest)
{
	double dx = source->getX () - dest->getX ();
	double dy = source->getY () - dest->getY ();
	double dz = source->getZ () - dest->getZ ();
	double distance = sqrt (dx*dx + dy*dy + dz*dz);
	double delay = distance / SPEED_OF_LIGHT;
	return delay;
}

void
FreeSpaceBroadcastChannel::send_later (NetInterface *to, Packet *packet, double delay)
{

	if (delay > 0.0) {
		Scheduler::instance ().schedule (m_handler, new FreeSpaceChannelEvent (to, packet), delay);
	} else {
		to->sendUp (packet);
	}
}

void 
FreeSpaceBroadcastChannel::sendDown (Packet *packet, NetInterface *caller)
{

	// XXX possible leak?	Packet *p(packet->copy());

	std::list<NetInterface *>::iterator tmp;
	NodePosition source;
	caller->peekPosition (&source);
	/* We could add a simple propagation delay to each packet 
	 * being received here.
	 */
	for (tmp = m_interfaces.begin (); tmp != m_interfaces.end (); tmp++) {
		if ((*tmp) != caller) {
			NodePosition dest;
			(*tmp)->peekPosition (&dest);
			send_later (*tmp, packet->copy (), calc_delay (&source, &dest));
		}
	}
	Packet::free (packet);
}

void 
FreeSpaceBroadcastChannel::registerInterface (NetInterface *interface)
{
	m_interfaces.push_back (interface);
}


NetInterface *FreeSpaceBroadcastChannel::getInterface(int address) {
	std::list<NetInterface *>::iterator tmp(m_interfaces.begin());
	while (tmp != m_interfaces.end()) {
		if (address == (*tmp)->getIpAddress()) {
			return *tmp;
		}
		++tmp;
	}
	return 0;
}
