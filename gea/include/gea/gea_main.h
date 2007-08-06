#ifndef _GEA_MAIN_H__
#define _GEA_MAIN_H__


#ifdef _MSC_VER
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
  #endif
  #define DLLLOCAL
#else
#define DLLEXPORT __attribute__ ((visibility("default")))
#define DLLLOCAL  __attribute__ ((visibility("hidden")))
#endif


#if defined PIC
#  define GEA_MAIN(argc, argv)						\
    extern "C" DLLEXPORT int gea_main(int argc, const char * const * argv) 
#else
#  define GEA_MAIN(argc, argv) GEA_MAIN_XXX(MODULE_NAME, argc, argv)
#  define GEA_MAIN_XXX(modulename, argc, argv)				\
    extern "C" int modulename##_gea_main(int argc, const char * const * argv)

#endif 

#endif //GEA_MAIN_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
