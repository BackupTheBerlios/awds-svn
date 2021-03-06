/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*-  *
 *
 * Copyright (c) 2004 INRIA
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
 * Author: Mathieu Lacage, <mathieu.lacage@sophia.inria.fr>
 */

#include "bpsk-mode.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <iostream>


BPSKMode::BPSKMode (double signalSpread, double rate)
	: BaseTransmissionMode (signalSpread, rate)
{}
BPSKMode::~BPSKMode ()
{}

double 
BPSKMode::chunkSuccessRate (double snr, unsigned int nbits)
{
	double ber = BPSKBER (snr);	
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	double csr = pow (1 - ber, nbits);
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	return csr;
}

BPSKFECMode::BPSKFECMode (unsigned int dFree, unsigned int adFree, 
			  double signalSpread, double rate, double codingRate)
	: FECBaseTransmissionMode (signalSpread, rate, codingRate),
	  dFree_ (dFree),
	  adFree_ (adFree)
{}
BPSKFECMode::~BPSKFECMode ()
{}
double
BPSKFECMode::chunkSuccessRate (double snr, unsigned int nbits)
{
	//	std::cout << __PRETTY_FUNCTION__ << std::endl;
	double ber = BPSKBER (snr);
	//	std::cout << "ber: " << ber << std::endl;
	//printf ("%g\n", ber);
	if (ber == 0) {
		return 1;
	}
	/* only the first term */
	//printf ("dfree: %d, adfree: %d\n", dFree_, adFree_);
	double pd = calculatePd (ber, dFree_);
	//	std::cout << "pd: " << pd << std::endl;
	double pmu = adFree_ * pd;
	//	std::cout << "pmu: " << pmu << std::endl;
	if (1-pmu < 0) {
		return 0;
	}
	double pms = pow (1 - pmu, nbits);
	//	std::cout << "pms: " << pms << std::endl;
	//printf ("ber: %g -- pd: %g -- pmu: %g -- pms: %g\n", ber, pd, pmu, pms);
	//	std::cout << pms << std::endl;
	return pms;
}
