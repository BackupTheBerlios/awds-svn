

#include <gea/API.h>
#include <objrepos.h>

using namespace std;


extern "C"
int gea_main(int argc, const char * const *argv) {

  void * void_test;
  Test *test;


  ObjRepository& rep = ObjRepository::instance();

  if ( argc < 2) {

    GEA.dbg() << argc << " usage " << argv[0] << " <objectname> " << endl;
    return 1;

  }

  void_test = rep.getObj(argv[1]);
  if (!void_test) {
    GEA.dbg() << "cannot find object " << argv[1] << endl;
    return 1;
  }

  GEA.dbg() << "object has type \"" << rep.getType(argv[1]) << '"' << endl;
  test = (Test *)void_test;

  GEA.dbg() << test->get() << endl;


  return 0;
}
