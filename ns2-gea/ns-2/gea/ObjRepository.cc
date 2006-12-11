#include <gea/ObjRepository.h>
#include <gea/API.h>
#include <gea/ShadowEventHandler.h>

using namespace std;

ObjRepository * ObjRepository::inst = 0;

long ObjRepository::whichNode() const {
    return GEA.shadow->currentNode->nodeid();
}
    


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

