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

#ifndef NET_NODE_H
#define NET_NODE_H

#include <list>

class Packet;
class Agent;
class Classifier;
class NetInterface;
class NetInterfaceConstructor;
class BroadcastChannel;
class NsObject;

class NodePosition {
public:
	NodePosition ();
	double getX (void);
	double getY (void);
	double getZ (void);
	void setX (double x);
	void setY (double y);
	void setZ (double z);
private:
	double m_x;
	double m_y;
	double m_z;
};


class NetNode {
public:
	NetNode ();
	virtual ~NetNode ();

	int getUid (void);

	void peekPosition (NodePosition *position);
	void setPosition (NodePosition *position);

	// XXX this should be something like sendDownToInterface
	void sendDown (Packet *packet);
	void receiveFromInterface (Packet *packet, NetInterface *interface);

	void attachAgent (Agent *agent);
	// <begin> jenz::inria
	void attachAgent(Agent *agent,int port);
	// <end> jenz::inria
	void addInterface (NetInterface *interface, BroadcastChannel *channel);

	NetInterface *getInterface() {
		return m_interface;
	}
private:
	int allocUid (void);

	static int m_uid;
	int m_selfUid;
	Classifier *m_demux;
	Classifier *m_classifier;
	NetInterface *m_interface;
	NsObject *m_interfaceConnector;

	NodePosition m_position;
};

#endif /* NODE_EMPTY_H */
