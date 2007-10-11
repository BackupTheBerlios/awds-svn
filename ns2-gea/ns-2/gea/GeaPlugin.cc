 
/* -*-	Mode:C++; c-basic-offset:4; tab-width:8; indent-tabs-mode:t -*- */
#include <gea/API.h>
#include <gea/GeaPlugin.h>
#include "ShadowEventHandler.h"
#include <iostream>
#include <dlfcn.h>
#include "Ns2ApiIface.h"

typedef int (*gea_main_t)(int argc, const char * const *argv);


void gea::initNs2ApiIface() {
    
    static bool ns2ApiInitilised = false; 
    
    if (ns2ApiInitilised) 
	return;
    
    GEA_apiIface = new Ns2ApiIface();
    GEA_apiIface->createSubEventHandler(&GEA);
    ns2ApiInitilised = true;
    cerr << "ns-2 gea api started" << endl;
}

int gea::gea_start(::TclObject *node, int argc,const char * const * argv) {

//    std::cerr << "Starting plugin: $filename" << endl;
    

    initNs2ApiIface();

    const char * filename = argv[1];
//    std::cerr << "Starting plugin: " << filename << endl;
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
    ShadowEventHandler *shadow = dynamic_cast<ShadowEventHandler *>(GEA.subEventHandler);
 
   
    shadow->setCurrentNode(node);
    GEA.lastEventTime = gea::AbsTime::now();
    int ret = (*gea_main)(argc - 1, &(argv[1]) );

    return ret;
}


