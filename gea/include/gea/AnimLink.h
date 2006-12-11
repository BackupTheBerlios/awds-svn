#ifndef _ANIMLINK_H__
#define _ANIMLINK_H__

#include <iostream>

namespace gea {
  
  struct AnimAddLink {
    
    const int dest;
    const char * const color;

    AnimAddLink(int dest, const char *color = "black") :
      dest(dest) ,color(color) {}
  
  };

  struct AnimDelLink {

    const int dest;
  
    AnimDelLink(int dest) :
      dest(dest) {}
  };

}



std::ostream& operator<<(std::ostream& os, const gea::AnimAddLink& l);
std::ostream& operator<<(std::ostream& os, const gea::AnimDelLink& l);


#endif //ANIMLINK_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
