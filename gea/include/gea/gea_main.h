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

#define GEA_MAIN(argc, argv)						\
    extern "C" DLLEXPORT int gea_main(int argc, const char * const * argv)

#if defined PIC
#  define GEA_MAIN_2(modulename, argc, argv)				\
     extern "C" DLLEXPORT int gea_main(int argc, const char * const * argv)
#else
#  define GEA_MAIN_2(modulename, argc, argv)				\
     extern "C" DLLEXPORT int modulename##_gea_main(int argc, const char * const * argv)

#endif

/** \def GEA_MAIN_2(modulename, argc, argv)
 *  \brief define the entry point of a GEA module
 *  \param modulename the name of the module
 *  \param argc       will be the first parameter of the entry funtion, of type int.
 *  \param argv       will be the second parameter of the entry funtion, of type const * const * char.
 *  \ingroup GEA_API
 */



#endif //GEA_MAIN_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
