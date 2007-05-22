#include <gea/API.h>
#include <gea/posix/ShadowEventHandler.h>
#include <gea/posix/UnixFdHandle.h>
#include <gea/posix/PosixApiIface.h>

#include <cstdlib>

#include <iostream>
#include <ltdl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;
using namespace gea;

typedef int (*gea_main_t)(int argc, const char * const *argv);

int conf_fd = 0;


int run_gea_main(int argc, char **argv) {
    
    
    
    lt_dlhandle handle = lt_dlopenext(argv[0]);
    if (!handle) {
	std::cerr << "Cannot open library: " << lt_dlerror() << '\n';
	return 1;
    }

    gea_main_t gea_main = (gea_main_t) lt_dlsym(handle, "gea_main");
    
    if (gea_main == 0) {
      std::cerr  << "warning: trying C++ ABI resolution of gea_main" << std::endl;
      gea_main = (gea_main_t) lt_dlsym(handle, "_Z8gea_mainiPKPKc");
      
    }

    if (gea_main == 0) {
      std::cerr << "Cannot load symbols: " << lt_dlerror() << '\n';
      return 1;
    }
        
    return (*gea_main)(argc, &(argv[0]) );
}



void parse_cmd(char *cmd) {
    //cout << "parsing :" << cmd <<":"<< endl;
    if (*cmd == '#') {
	//cout << "a comment" << endl;
	return;
    }
    
    int argc = 0;
    char *argv[100];
    
    
    char *token;
    token = strtok(cmd, " \t");
    while (token) {
	if (*token == '\0')
	    continue;
	argv[argc++] = token;
	token =strtok(0, " \t");
    }
    argv[argc] = 0;
    // for (int i = 0; i < argc; ++i) cout << i << ". " << argv[i] << endl;
    if (argc >0)
	run_gea_main(argc,argv);
    
}



void new_input(Handle *h, AbsTime t, void *data) {
    
    static char *line = 0;
    static size_t length = 0;
    
    if (h->status != Handle::Ready) {
	gea::geaAPI().waitFor(h, t + Duration(1.), new_input, 0);
	return;

    }
    
    static const int bsize = 10;
    char buf[bsize];
    
    //    cout << "got data" << endl;
    int retval = h->read(buf,bsize);
    
    bool eof = false;
    if (retval <= 0) {
	eof = true;
	// insert fake newline at end of file
	buf[0]='\n'; 
	retval=1;
    }
    
    // append buffer to line...
    line = (char *)realloc(line, length + retval + 1); // resize the buffer
    memcpy( &line[length], buf, retval);       // append to line
    length += retval;
    line[length] = '\0';              // terminate the string
    

    // split line into seperate commands
   
    char *delim;
    //    cout << "line is :" << line << ":" << endl;
    while ( (delim = strpbrk(line,"\n;") ) != 0 ) {
	*delim = '\0'; // seperate string
	parse_cmd(line);
	
	length -= 1 + (delim - line);
	char * newline = (char*)malloc(length+1);
	strcpy(newline, delim + 1);
	
	free(line);
	line = newline;
    }
 
    if (eof)
	close(conf_fd);
    else
	gea::geaAPI().waitFor(h, t + Duration(1.), new_input, 0);
    
}




void interactive() {
    
    UnixFdHandle unixfd(conf_fd, ShadowHandle::Read);
    
    gea::geaAPI().waitFor(&unixfd, AbsTime::now() + Duration(1.), new_input, 0);
    
    static_cast<ShadowEventHandler *>(gea::geaAPI().subEventHandler)->run();

}


int main(int argc, char **argv) {

    if (lt_dlinit() != 0) {
	cerr << "cannot initialise libltdl:" << lt_dlerror() << endl;
	return 1;
    }
    

    initPosixApiIface();

    
    if (argc==2 && !strcmp(argv[1],"-i")) {
	
	interactive();
	lt_dlexit();
	return 0;
	
    } else if (argc==3 && !strcmp(argv[1],"-c")) {
	
	conf_fd = open(argv[2], O_RDONLY);
	if (conf_fd < 0) {
	    cerr << "cannot open file " << argv[2] << endl;
	    return 1;
	}
	interactive();
	//close(conf_fd);
	lt_dlexit();
	return 0;
	
    } else    if(!(argc > 1)) {
	std::cerr << "No argument given. Shared object to load is needed!" << endl
		  << "usage: " << argv[0] << " <gea_module> <params>" << endl
		  << "   or: " << argv[0] << " -i        # for interactive mode" << endl
		  << "   or: " << argv[0] << " -c <file> # for reading lines from a file "<< endl;
	
	
	return 1;
    }
    
    int ret = run_gea_main(argc -1, &argv[1]);
    
    static_cast<ShadowEventHandler *>(gea::geaAPI().subEventHandler)->run();

    lt_dlexit();
    return ret;
}

// Command:	g++ -rdynamic -o gea_start gea_start.cc -ldl

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
