#include <gea/ObjRepository.h>
#include <gea/API.h>
#include <gea/ShadowEventHandler.h>

using namespace std;

ObjRepository * ObjRepository::inst = 0;

long ObjRepository::whichNode() const {
    // <begin> jenz::inria
    //    return GEA.shadow->currentNode->nodeid();
    return GEA.shadow->getCurrentNodeID();
    // <end> jenz::inria
}
    


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

