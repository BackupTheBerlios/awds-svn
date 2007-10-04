/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 2005 INRIA
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Lqicense version 2 as 
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

#include "free-space-propagation.h"
#include "tworay_propagation.h"
#include "net-interface-80211.h"
#include "ll-arp.h"
#include "phy-80211.h"
#include "phy-ber.h"
#include "phy-snrt.h"
#include "phy-et.h"
#include "mac-low.h"
#include "arf-mac-stations.h"
#include "aarf-mac-stations.h"
#include "cr-mac-stations.h"
#include "mac-tx-middle.h"
#include "mac-rx-middle.h"
#include "mac-high-qap.h"
#include "mac-high-nqap.h"
#include "mac-high-qsta.h"
#include "mac-high-nqsta.h"
#include "mac-high-adhoc.h"
#include "broadcast-channel.h"
#include "net-node.h"
#include "mac-parameters.h"
#include <iostream>
#include <math.h>
#include "hdr-marp.h"
#include "hdr-mac-80211.h"



void NetInterface80211::resetRate(int toAddress) {
	if (stations()->addressExists(toAddress)) {		
		stations()->lookup(toAddress)->resetRate();
	}
}

int NetInterface80211::getPackCount(int toAddress) {
	if (stations()->addressExists(toAddress)) {		
		return stations()->lookup(toAddress)->getPackCount();
	} else {
		return 0;
	}	
}

double NetInterface80211::getEffectiveRate(int toAddress) {	
	if (stations()->addressExists(toAddress)) {		
		return stations()->lookup(toAddress)->getEffectiveRate();
	} else {
		return 0;
	}
}

double NetInterface80211::getTime(int toAddress) {	
	if (stations()->addressExists(toAddress)) {		
		return stations()->lookup(toAddress)->getTime();
	} else {
		return 0;
	}
}


NetInterface80211::NetInterface80211 ()
	: NetInterface ()
{
}
NetInterface80211::~NetInterface80211 ()
{}

int 
NetInterface80211::getBSSID (void)
{
	return m_bssid;
}

MacParameters *
NetInterface80211::parameters (void)
{
	return m_parameters;
}
MacTxMiddle *
NetInterface80211::txMiddle (void)
{
	return m_txMiddle;
}
MacRxMiddle *
NetInterface80211::rxMiddle (void)
{
	return m_rxMiddle;
}
MacHigh *
NetInterface80211::high (void)
{
	return m_high;
}
MacLow *
NetInterface80211::low (void)
{
	return m_low;
}
MacStations *
NetInterface80211::stations (void)
{
	return m_stations;
}
Phy80211 *
NetInterface80211::phy (void)
{
	return m_phy;
}


LLArp *
NetInterface80211::ll (void)
{
	return m_ll;
}

void 
NetInterface80211::connectTo (BroadcastChannel *channel, NetNode *node)
{
	m_node = node;
	m_channel = channel;
}
void 
NetInterface80211::sendUpToNode (Packet *packet)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	m_node->receiveFromInterface (packet, this);
}
void 
NetInterface80211::sendDownToChannel (Packet *packet)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	m_channel->sendDown (packet, this);
}
void 
NetInterface80211::sendDown (Packet *packet)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	m_ll->sendDown (packet);
}
void 
NetInterface80211::sendUp (Packet *packet)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	m_phy->sendUp (packet);
}
int32_t
NetInterface80211::getIpAddress (void)
{
	return m_node->getUid ();
}
int
NetInterface80211::getMacAddress (void)
{
	return m_node->getUid ();
}
void
NetInterface80211::peekPosition (NodePosition *position)
{
	// <begin> jenz::inria
	m_node->peekPosition(position);
	//	*position = m_position;
	// <end> jenz::inria
}


/***********************************************************
 *     The constructor and configuration class.
 ***********************************************************/

double dBmToW(double dBm) {
	double mW = pow(10.0,dBm/10.0);
	return mW / 1000.0;
}

NetInterfaceConstructor80211::NetInterfaceConstructor80211 ()
{
	m_macMode = MAC_80211_MODE_ADHOC;
	m_rateControlMode = MAC_80211_RATE_ARF;
	m_phyModel = PHY_80211_MODEL_ET;
	set80211a ();
	m_phyRxThreshold = -84;
	m_phyRxNoise = 10;
	//	m_phyTxPower = 0.2818;
	
	m_phyTxPower = dBmToW(10);
	m_propSystemLoss = 1.0;
	m_propRxGain = 1;
	m_propTxGain = 1;
	m_propFrequency = 5e9;
	m_arpAliveTimeout = 1200;
	m_arpDeadTimeout = 100;
	m_arpWaitReplyTimeout = 1;
}
NetInterfaceConstructor80211::~NetInterfaceConstructor80211 ()
{}

void 
NetInterfaceConstructor80211::setQap (void)
{
	m_macMode = MAC_80211_MODE_QAP;
}
void 
NetInterfaceConstructor80211::setQsta (int ap)
{
	m_macMode = MAC_80211_MODE_QSTA;
	m_apMacAddress = ap;
}
void 
NetInterfaceConstructor80211::setNqap (void)
{
	m_macMode = MAC_80211_MODE_NQAP;
}
void 
NetInterfaceConstructor80211::setNqsta (int ap)
{
	m_macMode = MAC_80211_MODE_NQSTA;
	m_apMacAddress = ap;
}
void 
NetInterfaceConstructor80211::setAdhoc (void)
{
	m_macMode = MAC_80211_MODE_ADHOC;
}

void
NetInterfaceConstructor80211::setCr (int dataMode, int ctlMode)
{
	m_rateControlMode = MAC_80211_RATE_CR;
	m_crDataMode = dataMode;
	m_crCtlMode = ctlMode;
}
void
NetInterfaceConstructor80211::setArf (void)
{
	m_rateControlMode = MAC_80211_RATE_ARF;
}
void
NetInterfaceConstructor80211::setAarf (void)
{
	m_rateControlMode = MAC_80211_RATE_AARF;
}

void 
NetInterfaceConstructor80211::set80211a (void)
{
	m_standard = STANDARD_80211_A;
	m_propFrequency = 5e9;
}

void 
NetInterfaceConstructor80211::setPropSystemLoss (double systemLoss)
{
	m_propSystemLoss = systemLoss;
}
void 
NetInterfaceConstructor80211::setPropTxGain (double txGain)
{
	m_propTxGain = txGain;
}
void 
NetInterfaceConstructor80211::setPropRxGain (double rxGain)
{
	m_propRxGain = rxGain;
}
void 
NetInterfaceConstructor80211::setPropFrequency (double frequency)
{
	m_propFrequency = frequency;
}
void 
NetInterfaceConstructor80211::setFreeSpacePropagationModel (void)
{
}


void 
NetInterfaceConstructor80211::setPhyRxTreshold (double rxThreshold)
{
	m_phyRxThreshold = rxThreshold;
}
void 
NetInterfaceConstructor80211::setPhyRxNoise (double rxNoise)
{
	m_phyRxNoise = rxNoise;
}
void 
NetInterfaceConstructor80211::setPhyTxPower (double txPower)
{
	m_phyTxPower = dBmToW(txPower);
}
void 
NetInterfaceConstructor80211::setPhyModelBer (void)
{
	m_phyModel = PHY_80211_MODEL_BER;
}
void 
NetInterfaceConstructor80211::setPhyModelSnrt (void)
{
	m_phyModel = PHY_80211_MODEL_SNRT;
}
void 
NetInterfaceConstructor80211::setPhyModelEt (void)
{
	m_phyModel = PHY_80211_MODEL_ET;
}

void 
NetInterfaceConstructor80211::setArpAliveTimeout (double aliveTimeout)
{
	m_arpAliveTimeout = aliveTimeout;
}
void 
NetInterfaceConstructor80211::setArpDeadTimeout (double deadTimeout)
{
	m_arpDeadTimeout = deadTimeout;
}
void 
NetInterfaceConstructor80211::setArpWaitReplyTimeout (double waitReplyTimeout)
{
	m_arpWaitReplyTimeout = waitReplyTimeout;
}



NetInterface80211 *
NetInterfaceConstructor80211::createInterface (void)
{
	NetInterface80211 *interface = new NetInterface80211 ();
	MacLow *low;
	MacTxMiddle *txMiddle;
	MacRxMiddle *rxMiddle;
	MacStations *stations;
	MacParameters *parameters;
	Phy80211 *phy;
	LLArp *ll;

	txMiddle = new MacTxMiddle ();
	rxMiddle = new MacRxMiddle ();
	low = new MacLow ();
	parameters = new MacParameters ();


	ll = new LLArp ();
	ll->setTimeouts (m_arpAliveTimeout, m_arpDeadTimeout, m_arpWaitReplyTimeout);

	switch (m_rateControlMode) {
	case MAC_80211_RATE_ARF:
		stations = new ArfMacStations ();
		break;
	case MAC_80211_RATE_AARF:
		stations = new AarfMacStations ();
		break;
	case MAC_80211_RATE_CR:
		stations = new CrMacStations (m_crDataMode, m_crCtlMode);
		break;
	}

	FreeSpacePropagation *propagation = new FreeSpacePropagation ();
//	FreeSpacePropagation *propagation = new TwoRayPropagation;
	propagation->setSystemLoss (m_propSystemLoss);
	propagation->setTxGain (m_propTxGain);
	propagation->setRxGain (m_propRxGain);
	propagation->setFrequency (m_propFrequency);

	switch (m_phyModel) {
	case PHY_80211_MODEL_BER:
		phy = new PhyBer ();
		break;
	case PHY_80211_MODEL_SNRT:
		phy = new PhySnrt ();
		break;
	case PHY_80211_MODEL_ET:
		phy = new PhyEt ();
		break;
	}

	assert (m_standard == STANDARD_80211_A);
	phy->configureStandardA ();
	phy->setRxThreshold (m_phyRxThreshold);
	phy->setRxNoise (m_phyRxNoise);
	phy->setTxPower (m_phyTxPower);
	phy->setPropagationModel (propagation);

	interface->m_ll = ll;
	interface->m_phy = phy;
	interface->m_bssid = m_apMacAddress;
	interface->m_txMiddle = txMiddle;	
	interface->m_rxMiddle = rxMiddle;
	interface->m_low = low;
	interface->m_stations = stations;
	interface->m_parameters = parameters;

	switch (m_macMode) {
	case MAC_80211_MODE_QAP: {
		MacHighQap *high = new MacHighQap ();
		interface->m_high = high;
		high->setInterface (interface);
	} break;
	case MAC_80211_MODE_NQAP: {
		MacHighNqap *high = new MacHighNqap ();
		interface->m_high = high;
		high->setInterface (interface);
	} break;
	case MAC_80211_MODE_QSTA: {
		MacHighQsta *high = new MacHighQsta (m_apMacAddress);
		interface->m_high = high;
		high->setInterface (interface);
	} break;
	case MAC_80211_MODE_NQSTA: {
		MacHighNqsta *high = new MacHighNqsta (m_apMacAddress);
		interface->m_high = high;
		high->setInterface (interface);
	} break;
	case MAC_80211_MODE_ADHOC: {
		MacHighAdhoc *high = new MacHighAdhoc ();
		interface->m_high = high;
		high->setInterface (interface);
	} break;
	}


	ll->setMac (interface->m_high);
	ll->setInterface (interface);
	phy->setMac (low);
	phy->setInterface (interface);
	rxMiddle->setInterface (interface);
	low->setInterface (interface);
	stations->setInterface (interface);
	parameters->setInterface (interface);
	propagation->setInterface (interface);

	return interface;
}

void NetInterface80211::setDuration(double d,Packet *p) {
	p_info info;
	//	std::cout << info.name(HDR_CMN(p)->ptype_) << std::endl;
	//	std::cout << HDR_CMN(p)->ptype_ << std::endl;
	int dest(HDR_MAC_80211(p)->getFinalDestination());
	if (dest != -1) {
		//		std::cout << "setD: " << getIpAddress() << 	"  " << HDR_MAC_80211(p)->getSource() << "   " << HDR_MAC_80211(p)->getDestination() << std::endl;
		ttdata[dest].duration = d;
		if (HDR_CMN(p)->ptype_ == 0) {
		}
	}
}

void NetInterface80211::setSuccess(bool s,Packet *p) {
	int dest(HDR_MAC_80211(p)->getFinalDestination());
	if (dest != -1) {
		//		std::cout << "setS: " << getIpAddress() << 	"  " << HDR_MAC_80211(p)->getSource() << "   " << HDR_MAC_80211(p)->getDestination() << std::endl;
		//		std::cout << "here we are" << std::endl;
			if (s) {
				double &t(ttdata[dest].transmitTime);
				double &a(ttdata[dest].alpha);
				double &d(ttdata[dest].duration);
				t = t*(1-a)+d*a;
				//				std::cout << "Time: " << ttdata[dest].transmitTime << std::endl;
				d = 0;
			}
		if (HDR_CMN(p)->ptype_ == 0) {
		}
	}
}
double NetInterface80211::getTransmitTime(unsigned int dest) {
	return ttdata[dest].transmitTime;
}
