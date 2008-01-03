#include <cstdlib>




#include <gea/API.h>
#include <gea/Blocker.h>
#include <gea/posix/ShadowEventHandler.h>
#include <gea/NodeColor.h>

using namespace gea;
using namespace std;

struct Periodic {
  int num;
  Blocker blocker;

  Periodic() :
    num(10),
    blocker()
  {}

};


const double period = 0.75;


void bing(Handle *h, AbsTime t, void *data) {

  Periodic *p = reinterpret_cast<Periodic*>(data);

  GEA.dbg() << "bing " << p->num << endl
	    << gea::NodeColor( (p->num % 2) ? "white" : "blue" );
  if ( --(p->num) )
    GEA.waitFor(h, t + Duration(period), bing, data);
}




extern "C"
int gea_main(int argc, const char * const *argv) {

  Periodic *p = new Periodic();

  if ( argc == 2 )
    p->num = atoi(argv[1]);


  GEA.dbg() << " *** gea test application *** " << endl << endl
	    << " testing periodic trigger " << endl
	    << " This will print 'bing' " << p->num <<" times." << endl
	    << " The optional parameter can be used to define a different number." << endl;





  GEA.waitFor(&(p->blocker), AbsTime::now() + Duration(period), bing, p);

  //  delete blocker;
  return 0;
}
