#ifndef _GEAPLUGIN_H__
#define _GEAPLUGIN_H__

// <begin> jenz::inria
// class Node;
class TclObject;
// <end> jenz::inria

namespace gea {

    // <begin> jenz::inria
    //    int gea_start(class ::Node *node, int argc, const char * const * argv);
    int gea_start(class ::TclObject *node,int argc,const char * const * argv);
    // <end> jenz::inria
}



#endif //GEAPLUGIN_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
