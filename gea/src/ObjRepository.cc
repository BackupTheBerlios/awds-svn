#include <gea/ObjRepository.h>
#include <gea/ApiIface.h>
#include <gea/gea_main.h>
using namespace std;

DLLEXPORT ObjRepository * ObjRepository::inst = 0;

DLLEXPORT long ObjRepository::whichNode() const {
    return GEA_apiIface->getCurrentNode();
    //    return 0;
}



/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
