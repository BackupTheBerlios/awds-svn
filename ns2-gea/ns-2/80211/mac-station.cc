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

#include "mac-station.h"
#include "mac-stations.h"

double MacStation::getTime() {
	return clk_last-clk_first;
}

double MacStation::getEffectiveRate() {
	if (packet_counter) {
		return (double)rate_sum/(double)packet_counter;
	}
	return 0;
}

int MacStation::getPackCount() {
	return packet_counter;
}


void MacStation::resetRate() {
	packet_counter = 0;
	rate_sum = 0;
}

MacStation::MacStation (MacStations *stations) :
	clk_first(-1),
	clk_last(-1),
	packet_counter(0),
	rate_sum(0),
	m_sequence (0),
	m_associated (false),
	m_stations (stations)
	
{
}


MacStation::~MacStation ()
{}

int
MacStation::getNModes (void)
{
	return m_stations->getNModes ();
}
bool 
MacStation::isAssociated (void)
{
	return m_associated;
}
void 
MacStation::recordAssociated (void)
{
	m_associated = true;
}
