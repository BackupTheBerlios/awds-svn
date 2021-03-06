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

#include "mac-queue-80211e.h"
#include "mac-parameters.h"
#include "packet.h"

using namespace std;

MacQueue80211e::MacQueue80211e ()
{}

MacQueue80211e::~MacQueue80211e ()
{}

void
MacQueue80211e::setParameters (MacParameters *parameters)
{
	m_parameters = parameters;
}

void 
MacQueue80211e::enqueue (Packet *packet)
{
	cleanup ();
	double now;
	now = Scheduler::instance ().clock ();
	m_queue.push_front (make_pair<Packet *, double> (packet, now));
}
void 
MacQueue80211e::enqueueToHead (Packet *packet)
{
	cleanup ();
	double now;
	now = Scheduler::instance ().clock ();
	m_queue.push_back (make_pair<Packet *, double> (packet, now));
}

void
MacQueue80211e::cleanup (void)
{
	PacketQueueRI tmp;
	double now;

	if (m_queue.empty ()) {
		return;
	}

	now = Scheduler::instance ().clock ();
	tmp = m_queue.rbegin ();
	while (tmp != m_queue.rend ()) {
		if ((*tmp).second + m_parameters->getMSDULifetime () > now) {
			break;
		}
		Packet::free ((*tmp).first);
		tmp++;
	}
	m_queue.erase (tmp.base (), m_queue.end ());
}

Packet *
MacQueue80211e::dequeue (void)
{
	cleanup ();
	if (!m_queue.empty ()) {
		Packet *packet;
		packet = m_queue.back ().first;
		m_queue.pop_back ();
		return packet;
	}
	return NULL;
}

Packet *
MacQueue80211e::peekNextPacket (void)
{
	cleanup ();
	if (!m_queue.empty ()) {
		Packet *packet;
		packet = m_queue.back ().first;
		return packet;
	} else {
		return 0;
	}
}

bool
MacQueue80211e::isEmpty (void)
{
	cleanup ();
	return m_queue.empty ();
}


int
MacQueue80211e::size (void)
{
	return m_queue.size ();
}

void
MacQueue80211e::flush (void)
{
	PacketQueueI tmp;
	for (tmp = m_queue.begin (); tmp != m_queue.end (); tmp++) {
		Packet::free ((*tmp).first);
	}
	m_queue.erase (m_queue.begin (), m_queue.end ());
}
