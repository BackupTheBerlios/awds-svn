
#include <iostream>
#include <gea/API.h>
#include <gea/posix/UnixFdHandle.h>

using namespace std;
using namespace gea;

void dot(Handle *h, AbsTime t, void *data) {

  cout << "." << endl;
  cout.flush();
  if (h->status == Handle::Ready) {
    char buf[1000];
    h->read(buf, 1000);
    cout << "got some input" << endl;
  }
  GEA.waitFor(h, AbsTime::now() + Duration(1.0), dot, 0);
}


extern "C"
int gea_main(int argc, const char * const *argv) {
  
  UnixFdHandle *h = new UnixFdHandle(0, gea::PosixModeRead);
  
  GEA.waitFor(h, AbsTime::now() + Duration(1.0), dot, 0);
  
  return 0;
}
