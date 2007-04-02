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

#include "net-node.h"
#include "tcl-net-node.h"
#include "tcl-broadcast-channel.h"
#include "tcl-net-interface.h"
#include "tcl-constructor.h"
#include "agent.h"

// <begin> jenz::inria
#include <gea/GeaPlugin.h>
// <end> jenz:inria

#include <iostream>


TCL_CONSTRUCTOR (TclNetNode, "TclNetNode");

unsigned int TclNetNode::id_count(0);

TclNetNode::TclNetNode ()
{
	m_node = new NetNode ();
	id = id_count++;
}

int 
TclNetNode::command(int argc, const char*const* argv)
{
	//	std::cout << argv[1] << std::endl;
	
	// <begin> jenz::inria
	if ((argc == 2) && (strcmp(argv[1],"reset") == 0)) {
		//		std::cout << "reset ";
		//		std::cout << "not yet implemented" << std::endl;
		return TCL_OK;
	}
	if ((argc == 2) && (strcmp(argv[1],"start") == 0)) {
		//		std::cout << "start ";
		//		std::cout << "not yet implemented" << std::endl;
		return TCL_OK;
	}
	if ((argc == 2) && (strcmp(argv[1],"id") == 0)) {
		//	std::cout << "cmd id" << std::endl;
		Tcl&        tcl     = Tcl::instance();
		char buf[100];
		sprintf(buf,"%i",id);
		tcl.result(buf);
		return TCL_OK;
	}
	/* check if gea_start is requested .... */
	if ( (argc > 2) && ( strcmp(argv[1], "gea_start") == 0) ) {
			int ret = gea::gea_start(this, argc - 1, &(argv[1]));
			if (ret == 0) 
			return TCL_OK;
		else 
			return TCL_ERROR;
	}
	// <end> jenz::inria
	if (argc == 5) {
		if (strcmp (argv[1], "set-position") == 0) {
			NodePosition pos;
			m_node->peekPosition (&pos);
			pos.setX (atof (argv[2]));
			pos.setY (atof (argv[3]));
			pos.setZ (atof (argv[4]));
			//			std::cout << pos.getX() << " " << pos.getY() << " " << pos.getZ() << std::endl;
			m_node->setPosition (&pos);
			//			std::cout << pos.getX() << " " << pos.getY() << " " << pos.getZ() << std::endl;
			m_node->peekPosition(&pos);
			return TCL_OK;
		}
		// <begin> jenz::inria
		if (strcmp(argv[1],"setdest")== 0) {
			return TCL_OK;
		}
		// <end> jenz::inria
	} else if (argc == 4) {
		if (strcmp (argv[1], "add-interface") == 0) {
			TclNetInterface *interface = static_cast <TclNetInterface *> (lookup (argv[2]));
			TclBroadcastChannel *channel = static_cast <TclBroadcastChannel *> (lookup (argv[3]));
			m_node->addInterface (interface->getInterface (), channel->getChannel ());
			return TCL_OK;
		}
		// <begin> jenz::inria
		if (strcmp(argv[1], "attach") == 0) {
			/*			for (int i(0);i<argc;++i) {
				std::cout << argv[i] << std::endl;
				}*/
			Agent *agent = static_cast <Agent *> (lookup(argv[2]));
			m_node->attachAgent(agent,atoi(argv[3]));
		return TCL_OK;
		}
		// <end> jenz::inria
	} else if (argc == 3) {
		if (strcmp (argv[1], "attach-agent") == 0) {
			Agent *agent = static_cast <Agent *> (lookup (argv[2]));
			cout << (int) agent << std::endl;
			m_node->attachAgent (agent);
			return TCL_OK;
		}
		// <begin> jenz::inria
		if (strcmp(argv[1], "attach") == 0) {
			/*			for (int i(0);i<argc;++i) {
				std::cout << argv[i] << std::endl;
				}*/
			Agent *agent = static_cast <Agent *> (lookup(argv[2]));
			m_node->attachAgent(agent);
			return TCL_OK;
		}
		if (strcmp(argv[1], "color") == 0) {
			//			std::cout << "color ";
			//			std::cout << "not yet implemented" << std::endl;
			return TCL_OK;
		}
		if (strcmp(argv[1],"random-motion") == 0) {
			//			std::cout << "random-motion ";
			//			std::cout << "not yet implemented" << std::endl;
			return TCL_OK;
		}
		// <end> jenz::inria
	} 
	std::cout << "unknown command" << std::endl;
	return TclObject::command (argc, argv);
}
