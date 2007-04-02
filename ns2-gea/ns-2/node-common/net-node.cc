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

#include "classifier.h"
#include "agent.h"

#include "net-node.h"
#include "broadcast-channel.h"
#include "net-interface.h"

#include <iostream>

namespace {
	class InterfaceConnector : public NsObject {
	public:
		InterfaceConnector (NetNode *node)
			: m_node (node) {}
		virtual ~InterfaceConnector () {}

		virtual void recv(Packet*packet, Handler* callback = 0) {
			// XXX we don't deal with packet callbacks here.
			//			std::cout << __PRETTY_FUNCTION__ << std::endl;
			m_node->sendDown (packet);
		}
	private:
		NetNode *m_node;
	};
}

NodePosition::NodePosition ()
	: m_x (0.0), m_y (0.0), m_z (0.0)
{}
double 
NodePosition::getX (void)
{
	return m_x;
}
double 
NodePosition::getY (void)
{
	return m_y;
}
double 
NodePosition::getZ (void)
{
	return m_z;
}

void 
NodePosition::setX (double x)
{
	m_x = x;
}
void 
NodePosition::setY (double y)
{
	m_y = y;
}
void 
NodePosition::setZ (double z)
{
	m_z = z;
}



int NetNode::m_uid = 0;

NetNode::NetNode ()
	: m_selfUid (allocUid ()),
	  m_demux (0)
{
	m_interfaceConnector = new InterfaceConnector (this);
	m_classifier = static_cast <Classifier *> (TclObject::New ("Classifier/Addr"));
	m_demux = static_cast <Classifier *> (TclObject::New ("Classifier/Port"));
	// add notification route ?
}

NetNode::~NetNode ()
{}

int
NetNode::allocUid (void)
{
	m_uid++;
	return m_uid;
}

int
NetNode::getUid (void)
{
	return m_selfUid;
}

void 
NetNode::peekPosition (NodePosition *position)
{
	*position = m_position;
}

void
NetNode::setPosition (NodePosition *position)
{
	m_position = *position;
}

void
NetNode::sendDown (Packet *packet)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	m_interface->sendDown (packet);
}

void 
NetNode::receiveFromInterface (Packet *packet, NetInterface *interface)
{
	/* When we receive a packet from the low-level layers, 
	 * we forward it to the port demuxer directly.
	 */
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	m_demux->recv (packet, 0);
}

void
NetNode::attachAgent (Agent *agent)
{
	Agent *nullAgent = static_cast <Agent *> (TclObject::New ("Agent/Null"));
	int port = m_demux->allocPort (nullAgent);
	agent->port () = port;
	agent->addr () = getUid ();
	agent->target (m_interfaceConnector);
	m_demux->install (port, agent);
}

// <begin> jenz::inria
void NetNode::attachAgent(Agent *agent,int port) {
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	agent->port() = port;
	agent->addr() = getUid();
	agent->target(m_interfaceConnector);
	m_demux->install(port,agent);
}
// <end> jenz::inria

void
NetNode::addInterface (NetInterface *interface, BroadcastChannel *channel)
{
	m_interface = interface;
	m_interface->connectTo (channel, this);
	channel->registerInterface (m_interface);
}

