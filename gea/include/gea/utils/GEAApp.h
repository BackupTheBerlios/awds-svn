#ifndef __GEAAPP_H_
#define __GEAAPP_H_

class GEAApp {
public:
    GEAApp() {};
    virtual ~GEAApp() {};

    virtual void start() = 0;
};

typedef GEAApp* create_t();
typedef void destroy_t(GEAApp*);

#endif // __GEAAPP_H_

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
