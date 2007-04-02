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

#include <tcl.h>

#include "packet-header-manager.h"
#include "scheduler.h"
#include "simple-broadcast-channel.h"
#include "net-node.h"
#include "net-interface-80211.h"
#include "tcp.h"
#include "tcp-sink.h"
#include "app.h"

#include "mem-trace.h"

extern void init_misc(void);
extern EmbeddedTcl et_ns_lib;
extern EmbeddedTcl et_ns_ptypes;
#ifdef MEMDEBUG_SIMULATIONS
MemTrace *globalMemTrace;
#endif

static void
ip_connect (Agent *source, Agent *dest)
{
	source->daddr () = dest->addr ();
	source->dport () = dest->port ();
	dest->daddr () = source->addr ();
	dest->dport () = source->port ();
}

class StartHandler : public Handler {
public:
	StartHandler (Application *application)
		: m_application (application)
	{}

	virtual void handle(Event* event) {
		m_application->send (-1);
		delete event;
	}

private:
	Application *m_application;
};

class EndHandler : public Handler {
public:
	EndHandler (Scheduler *scheduler)
		: m_scheduler (scheduler)
	{}

	virtual void handle(Event* event) {
		m_scheduler->halt ();
		delete event;
	}
private:
	Scheduler *m_scheduler;
};

static void
create_my_topology (void)
{
	NetNode *nodes[3];
	BroadcastChannel *channel;
	NodePosition position;
	NetInterfaceConstructor80211 *interfaceConstructor;
	NetInterface80211 *interfaces[3];
	TcpSink *sink;
	TcpAgent *source;
	Application *ftp;	
	HeapScheduler *scheduler;

	PacketHeaderManager::instance ()->initializePacketOffsets ();

	scheduler = static_cast <HeapScheduler *> (TclObject::New ("Scheduler/Heap"));
	Scheduler::instance (scheduler);

	channel = new SimpleBroadcastChannel ();
	interfaceConstructor = new NetInterfaceConstructor80211 ();

	nodes[0] = new NetNode ();
	nodes[1] = new NetNode ();
	nodes[2] = new NetNode ();

	position.setX (0.0);
	position.setY (0.0);
	position.setZ (0.0);
	nodes[0]->setPosition (&position);
	position.setX (200.0);
	position.setY (0.0);
	position.setZ (0.0);
	nodes[0]->setPosition (&position);
	position.setX (0.0);
	position.setY (200.0);
	position.setZ (0.0);
	nodes[0]->setPosition (&position);

	interfaceConstructor->setNqap ();
	interfaces[0] = interfaceConstructor->createInterface ();
	interfaceConstructor->setNqsta (1);
	interfaces[1] = interfaceConstructor->createInterface ();
	interfaces[2] = interfaceConstructor->createInterface ();

	nodes[0]->addInterface (interfaces[0], channel);
	nodes[1]->addInterface (interfaces[1], channel);
	nodes[2]->addInterface (interfaces[2], channel);

	sink = static_cast <TcpSink *> (TclObject::New ("Agent/TCPSink"));
	nodes[1]->attachAgent (sink);
	if (TclObject::New ("Agent/TCP") == 0) {
		printf ("baad\n");
	}
	source = static_cast <TcpAgent *> (TclObject::New ("Agent/TCP"));
	source->fid (2); // in tcl, class 2
	nodes[2]->attachAgent (source);
	ftp = static_cast<Application *> (TclObject::New ("Application"));
	ftp->attachAgent (source);
	ip_connect (source, sink);

	scheduler->schedule (new StartHandler (ftp), new Event (), 3.0);
	scheduler->schedule (new EndHandler (scheduler), new Event (), 5.0);

	scheduler->run ();

	delete ftp;
	delete sink;
	delete source;
	delete nodes[0];
	delete nodes[1];
	delete nodes[2];
	delete interfaceConstructor;
	delete channel;
}



static int 
initialize (Tcl_Interp *interp)
{
#ifdef MEMDEBUG_SIMULATIONS
	globalMemTrace = new MemTrace;
#endif

	if (Tcl_Init(interp) == TCL_ERROR ||
	    Otcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
	}

	Tcl_SetVar(interp, "tcl_rcFileName", "~/.ns.tcl", TCL_GLOBAL_ONLY);
	Tcl::init(interp, "ns");
	init_misc();
        et_ns_ptypes.load();
	et_ns_lib.load();

	create_my_topology ();

	return TCL_OK;
}

int main (int argc, char *argv[])
{
	Tcl_Main(argc, argv, initialize);


	return 0;
}
