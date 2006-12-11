
#include <gea/API.h>
#include <gea/posix/ShadowEventHandler.h>


extern "C"
int gea_main(int argc, const char * const *argv);


int main(int argc, const char **argv) {
  
  int ret;

  ret = gea_main(argc,argv);
  
  gea::geaAPI().shadow->run();
  
  return ret;
}


