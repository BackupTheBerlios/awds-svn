/* -*-	Mode:C++; c-basic-offset:4; tab-width:8; indent-tabs-mode:t -*- */


#include <gea/API.h>

#include <apps/app.h>
#include <apps/udp.h>
#include <routing/address.h>
#include <common/simulator.h>
#include <common/packet.h>

#include <gea/UdpHandle.h>
#include "ShadowUdpHandle.h"
#include "ShadowHandle.h"
#include "Ns2ApiIface.h"
#include <iostream>


#include <cassert>
//#include <assert.h>

using namespace std;

//const char *gea::UdpAddress::IP_ANY       = "255";
//const char *gea::UdpAddress::IP_BROADCAST = "255.255.255.255";

// gea::UdpAddress::UdpAddress(int port, const char *ip) : 
//     shadow(new gea::ShadowUdpAddress(port, ip) )
// {
    
// }

// gea::UdpAddress::UdpAddress(const gea::UdpAddress& a) :
//     shadow(new gea::ShadowUdpAddress( *(a.shadow) ) )
// { 
// }


// gea::UdpAddress::UdpAddress(u_int32_t ip, u_int16_t port) :
//     shadow(new gea::ShadowUdpAddress( port, gea::UdpAddress::IP_BROADCAST  ))

// {
//     setIP(ip);
//     setPort(port);
// }

// gea::UdpAddress& gea::UdpAddress::operator=(const gea::UdpAddress& a) {
//     *(this->shadow) = *(a.shadow);
//     return *this;
// }

// gea::UdpAddress::~UdpAddress() {
//     delete shadow;
// }


// u_int32_t gea::UdpAddress::getIP() const {
//     return shadow->ip;
// }

// void gea::UdpAddress::setIP(u_int32_t ip) {
//     shadow->ip = ip;
// }

// u_int16_t gea::UdpAddress::getPort() const {
//     return shadow->port;
// }

// void gea::UdpAddress::setPort(u_int16_t port) {
//     shadow->port = port;
// }
	


static gea::ShadowUdpHandle *
makeShadowUdpHandle(gea::UdpHandle        *h,
		    gea::UdpHandle::Mode   mode,
		    const gea::UdpAddress& addr ) {
    
    Tcl& tcl = Tcl::instance();
    
    /* store parameters in global variables */
    gea::ShadowUdpHandle::currentUdpHandle  = h;
    gea::ShadowUdpHandle::currentMode       = mode;
    gea::ShadowUdpHandle::currentUdpAddress = addr; 

    /* create object by calling the tcl interpreter */
    tcl.evalf("new Gea/ShadowUdpHandle");
    
    /* receive the resulting tcl object */
    return dynamic_cast<gea::ShadowUdpHandle *>(TclObject::lookup(tcl.result()));
    
}


// gea::UdpHandle::UdpHandle(gea::UdpHandle::Mode   mode,
// 			  const gea::UdpAddress& addr ) :
//     shadowUdpHandle( makeShadowUdpHandle(this, mode, addr) )
// {
//     assert(this->shadowUdpHandle);
//     assert(this->shadowHandle->isUdpHandle() );
//     this->shadowUdpHandle->init();
//     //   cout << "shadowUdpHandle has name " << this->shadowUdpHandle->name() << endl;
// }

void gea::Ns2ApiIface::createSubUdpHandle(UdpHandle *udpHandle,
					  bool       receiveMode, 
					  const      UdpAddress& addr) 
{
    
    ShadowUdpHandle *shadow  = makeShadowUdpHandle( udpHandle, 
						   receiveMode ? gea::UdpHandle::Recv : gea::UdpHandle::Send,
						   addr );
    udpHandle->subUdpHandle = shadow;
    shadow->init();
}
	
// gea::UdpHandle::~UdpHandle() {
//     delete shadowUdpHandle;
// } 


void gea::Ns2ApiIface::destroySubUdpHandle(UdpHandle *udpHandle) {
    assert (udpHandle->subUdpHandle);
    delete udpHandle->subUdpHandle;
    udpHandle->subUdpHandle = 0;
}
/* 
   int gea::UdpHandle::read (char *buf, int size) {
    
   return this->shadowUdpHandle->read(buf, size);
   }

   int gea::UdpHandle::setSrc(gea::UdpAddress src_addr) {
    
   return this->shadowUdpHandle->setSrc(src_addr);
   }

   void gea::UdpHandle::setDest(gea::UdpAddress dest_addr) {
   this->shadowUdpHandle->setDest(dest_addr);
   }

   int gea::UdpHandle::write(const char *buf, int size) {
    
   return this->shadowUdpHandle->write(buf, size);
   }

   gea::UdpAddress gea::UdpHandle::getSrc() const {

   return this->shadowUdpHandle->src;
   }
	
*/

//unsigned long gea::UdpHandle::getIP() {
// <begin> jenz::inria
//    return (unsigned long)GEA.shadow->currentNode->nodeid();

// <end> jenz::inria
// }

long gea::Ns2ApiIface::getCurrentNode() {
    ShadowEventHandler *shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler);
    return shadow->getCurrentNodeID();
}

uint32_t gea::Ns2ApiIface::getIpAddr() {
    ShadowEventHandler *shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler);
    return (unsigned long) shadow->getCurrentNodeID();
}

