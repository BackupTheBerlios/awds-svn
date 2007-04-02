#include "tworay_propagation.h"

#include "net-node.h"
#include "hdr-mac-80211.h"
#include <node-common/net-interface.h>
#include <math.h>

#include <iostream>


TwoRayPropagation::TwoRayPropagation():FreeSpacePropagation() {
	std::cout << "TwoRay" << std::endl;
}

double TwoRayPropagation::TwoRay(double Pt, double Gt, double Gr, double ht, double hr, double L, double d) {
        /*
         *  Two-ray ground reflection model.
         *
         *	     Pt * Gt * Gr * (ht^2 * hr^2)
         *  Pr = ----------------------------
         *           d^4 * L
         *
         * The original equation in Rappaport's book assumes L = 1.
         * To be consistant with the free space equation, L is added here.
         */
  return Pt * Gt * Gr * (hr * hr * ht * ht) / (d * d * d * d * L);
}


double TwoRayPropagation::getReceptionPower (Packet *packet) {
	std::cout << "getReceptionPower" << std::endl;
	double txAirPower = HDR_MAC_80211 (packet)->getTxAirPower ();
	double L = m_systemLoss;			// system loss
	double lambda = getLambda();	// wavelength

	NodePosition txPosition,rxPosition;
	HDR_MAC_80211 (packet)->peekTxNodePosition(&txPosition);
	m_interface->peekPosition(&rxPosition);
	double sqDistance = squareDistance(&txPosition,&rxPosition);
	double d(sqrt(sqDistance));
	
  /* We're going to assume the ground is essentially flat.
     This empirical two ground ray reflection model doesn't make 
     any sense if the ground is not a plane. */

	if (rxPosition.getZ() != txPosition.getZ()) {
		printf("%s: TwoRayGround propagation model assume flat ground\n",__FILE__);
	}

	double hr = rxPosition.getZ();// rZ + r->getAntenna()->getZ();
	double ht = txPosition.getZ();//tZ + t->getAntenna()->getZ();

	if (hr != last_hr || ht != last_ht) { // recalc the cross-over distance
      /* 
	         4 * PI * hr * ht
	 d = ----------------------------
	             lambda
	   * At the crossover distance, the received power predicted by the two-ray
	   * ground model equals to that predicted by the Friis equation.
       */

		crossover_dist = (4 * PI * ht * hr) / lambda;
		last_hr = hr; last_ht = ht;
#if DEBUG > 3
		printf("TRG: xover %e.10 hr %f ht %f\n",crossover_dist, hr, ht);
#endif
	}

  /*
   *  If the transmitter is within the cross-over range , use the
   *  Friis equation.  Otherwise, use the two-ray
   *  ground reflection model.
   */

		double Gt = m_txGain; // t->getAntenna()->getTxGain(rX - tX, rY - tY, rZ - tZ, t->getLambda());
		double Gr = m_rxGain; // r->getAntenna()->getRxGain(tX - rX, tY - rY, tZ - rZ, r->getLambda());


	if(d <= crossover_dist) {
		double Pr = Friis(txAirPower, Gt, Gr, lambda, L, sqDistance);
#if DEBUG > 3
		printf("Friis %e\n",Pr);
#endif
	    return Pr;
  	} else {
    		double Pr = TwoRay(txAirPower, Gt, Gr, ht, hr, L, d);
#if DEBUG > 3
		printf("TwoRay %e\n",Pr);
#endif    
    		return Pr;
  	}
}
