#ifndef __GEA_API_H_
#define __GEA_API_H_

#include <gea/EventHandler.h>
#include <gea/Blocker.h>

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
