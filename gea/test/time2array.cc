
#include <gea/Time.h>
#include <iostream>

using namespace gea;
using namespace std;

extern "C"
int gea_main() {

  char  buf[AbsTime::size];

  AbsTime t1 = AbsTime::now();
  AbsTime t2;

  for (int i = 0; i != 100000; ++i) {
    cout << "";
  }

  t2 = AbsTime::now();


  cout << " v1 = " << (t2 - t1) << endl;
  (t2-t1).toArray(buf);

  Duration d;
  d.fromArray(buf);
  cout << " v1'= " << d << endl;

  (t1-t2).toArray(buf);
  d.fromArray(buf);
  cout << "-v1'= " << d << endl;

  t2.toArray(buf);
  t2.fromArray(buf);
  cout << " v1 = " << (t2 - t1) << endl;


  return 0;
}
