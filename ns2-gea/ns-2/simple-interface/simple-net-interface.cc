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

#include "scheduler.h"
#include "packet.h"

#include "simple-net-interface.h"
#include "broadcast-channel.h"
#include "net-node.h"

#define SIMPLE_TRACE 1

#ifdef SIMPLE_TRACE
# define TRACE(format, ...) \
	printf ("SIMPLE TRACE %d %f " format "\n", getMacAddress (), \
                Scheduler::instance ().clock (), ## __VA_ARGS__);
#else /* SIMPLE_TRACE */
# define TRACE(format, ...)
#endif /* SIMPLE_TRACE */


class Delay : public Handler {
public:
	Delay (SimpleNetInterface *interface, double delay) 
		: m_interface (interface),
		  m_delay (delay) 
	{}
	
	void start (Packet *packet) {
		Scheduler::instance ().schedule (this, packet, m_delay);
	}
	
	virtual void handle(Event* event) {
		Packet *packet = static_cast <Packet *> (event);
		m_interface->sendDownToChannel (packet);
	}
private:
	SimpleNetInterface *m_interface;
	double m_delay;
};


SimpleNetInterface::SimpleNetInterface (double delay)
	: NetInterface ()
	  
{
	m_delay = new Delay (this, delay);
}
SimpleNetInterface::~SimpleNetInterface ()
{}

void 
SimpleNetInterface::connectTo (BroadcastChannel *channel, NetNode *node)
{
	m_channel = channel;
	m_channel->registerInterface (this);
	m_node = node;
}
void 
SimpleNetInterface::sendDown (Packet *packet)
{
	TRACE ("send down");
	m_delay->start (packet);
}
void 
SimpleNetInterface::sendUp (Packet *packet)
{
	TRACE ("send up");
	sendUpToNode (packet);
}
void 
SimpleNetInterface::sendUpToNode (Packet *packet)
{
	m_node->receiveFromInterface (packet, this);
}
void 
SimpleNetInterface::sendDownToChannel (Packet *packet)
{
	TRACE ("down to channel");
	m_channel->sendDown (packet, this);
}
int32_t
SimpleNetInterface::getIpAddress (void)
{
	return m_node->getUid ();
}
int
SimpleNetInterface::getMacAddress (void)
{
	return m_node->getUid ();
}
void
SimpleNetInterface::peekPosition (NodePosition *position)
{
	*position = m_position;
}



SimpleNetInterfaceConstructor::SimpleNetInterfaceConstructor ()
	: m_delay (1e-3)
{}
SimpleNetInterfaceConstructor::~SimpleNetInterfaceConstructor ()
{}

void 
SimpleNetInterfaceConstructor::setDelay (double delay)
{
	m_delay = delay;
}

SimpleNetInterface *
SimpleNetInterfaceConstructor::createInterface (void)
{
	return new SimpleNetInterface (m_delay);
}
