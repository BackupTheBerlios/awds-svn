#ifndef _NODECOLOR_H__
#define _NODECOLOR_H__

#include <ostream>

namespace gea {

    struct NodeColor {
	const char *color;
	NodeColor(const char *color) : color(color) {}
    };
}

std::ostream& operator<<(std::ostream& os, const gea::NodeColor& nc);

#endif //NODECOLOR_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
