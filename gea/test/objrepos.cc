
#include <gea/API.h>
#include <objrepos.h>

using namespace std;


extern "C"
int gea_main(int argc, const char * const *argv) {

  void * void_test;
  Test *test;
  bool success;

  ObjRepository& rep = ObjRepository::instance();

  test = new Test(4711);
  success = rep.insertObj("test4711", "Test", test);
  assert (success);

  test = new Test(4712);
  success = rep.insertObj("test4712", "Test", test);
  assert (success);


  test = 0;

  void_test = rep.getObj("test4711");
  assert(void_test != 0);
  test = reinterpret_cast<Test *>(void_test);
  GEA.dbg() << test->get() << endl;

  void_test = rep.getObj("test4712");
  assert(void_test != 0);
  test = reinterpret_cast<Test *>(void_test);
  GEA.dbg() << test->get() << endl;


  return 0;
}
