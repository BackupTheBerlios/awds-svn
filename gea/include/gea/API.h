#ifndef __GEA_API_H_
#define __GEA_API_H_

#include <gea/EventHandler.h>
#include <gea/Blocker.h>


/** 
    \mainpage GEA Documentation
    
    \section overview Overview
    
    GEA is a not very lucky acronym for Generic Event-based API. It
    was developed as a helper library for the development of routing protocols 
    for Wirelss Mesh Networks. When being used, the created code can be used in the 
    network simulator NS-2 as well as in a real system. This allows a very fast development cycle 
    by firstly testing all modification in a simulated network, before deploying 
    it to the real mesh network. 

    Even though GEA was developed for mesh networks, it is not limited to this. 
    GEA provides an abstract view on the select() system call, some people may find this useful. 

    \section gea_api The GEA Programming Interface

    The programming interfaces of GEA consists of only a small set of classes, which can 
    be found in the documentation module \link GEA_API \endlink.
    
    \section build_modules Building a Modular Architecture with GEA
    
    A GEA based software consists of a set of modules. A module is normally implemented as a 
    shared library. 
    
    \li \link GEA_MAIN_2 \endlink
    \li \link ObjRepository \endlink
    
    \section ext_GEA Extending GEA
    
    You can extend the GEA-API in a platform specific way by using the
    native \link PosixAPI \endlink. It provides access to the internal \link gea::UnixFdHandle \endlink that can 
    be used for handling events generated by Unix file descriptors. 


    
*/

/** \defgroup GEA_API 
 * 
 *   \brief This includes all user interfaces of the API.
 */

/** \brief Access the GEA interface by this global variable
 *  \ingroup GEA_API
 */
extern gea::EventHandler GEA;

namespace gea {
  
  gea::EventHandler& geaAPI();
  
}
#endif // __GEA_API_H_
