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

#ifndef FREE_SPACE_BROADCAST_CHANNEL_H
#define FREE_SPACE_BROADCAST_CHANNEL_H

#include <list>
#include "broadcast-channel.h"

class Packet;
class NetInterface;
class NodePosition;

class FreeSpaceBroadcastChannel : public BroadcastChannel {
public:
	FreeSpaceBroadcastChannel ();
	virtual ~FreeSpaceBroadcastChannel ();

	virtual void sendDown (Packet *packet, NetInterface *caller);

	virtual void registerInterface (NetInterface *interface);

	virtual NetInterface *getInterface(int address);
private:
	double calc_delay (NodePosition *source, NodePosition *dest);
	void send_later (NetInterface *to, Packet *packet, double delay);
	std::list<NetInterface *> m_interfaces;
	class FreeSpaceChannelHandler *m_handler;
};

#endif /* FREE_SPACE_BROADCAST_CHANNEL_H */

