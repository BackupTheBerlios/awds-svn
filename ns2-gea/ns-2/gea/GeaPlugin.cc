/* -*-	Mode:C++; c-basic-offset:4; tab-width:8; indent-tabs-mode:t -*- */
#include <gea/API.h>
#include <gea/GeaPlugin.h>
#include <gea/ShadowEventHandler.h>
#include <iostream>
#include <dlfcn.h>


typedef int (*gea_main_t)(int argc, const char * const *argv);


int gea::gea_start(::TclObject *node, int argc,const char * const * argv) {

    //    std::cerr << "Starting plugin: $filename" << endl;

    const char * filename = argv[1];
    void *dl_handle = dlopen(filename, RTLD_NOW);

    if (!dl_handle) {
	std::cerr << "Cannot open shared library: " << filename << std::endl 
		  << "error:  " << dlerror() << std::endl;
	return -1;
    }
    
    gea_main_t gea_main = (gea_main_t)(dlsym(dl_handle, "gea_main")); 
    
    if (gea_main == 0) {
	std::cerr  << "warning: trying C++ ABI resolution of gea_main" << std::endl;
	gea_main = (gea_main_t)(dlsym(dl_handle, "_Z8gea_mainiPKPKc"));
    }

    if ( gea_main == 0) {
	std::cerr << "Cannot load symbol gea_main from shared object: " << dlerror() << '\n';
	dlclose(dl_handle);
	return -1;
    }

    GEA.shadow->currentNode = node;
    GEA.lastEventTime = gea::AbsTime::now();
    int ret = (*gea_main)(argc - 1, &(argv[1]) );

    return ret;
}


