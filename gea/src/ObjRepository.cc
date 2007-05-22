#include <gea/ObjRepository.h>
#include <gea/ApiIface.h>
using namespace std;

ObjRepository * ObjRepository::inst = 0;

long ObjRepository::whichNode() const {
    return GEA_apiIface->getCurrentNode();
    //    return 0;
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

