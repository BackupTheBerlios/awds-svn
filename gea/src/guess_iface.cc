#include <iostream>
#include <cassert>

#ifdef WIN32


void guess_iface(char *ret) {
  strcpy(ret, "255.255.255.255");
}


#else // not WIN32


#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//using namespace std;

static bool guess_iface(int level, char *ip_addr) {
  
  int fd = socket(PF_INET, SOCK_DGRAM, 0);
  assert(fd >= 0);
  
  static const int MAX_DEV = 12;

  struct ifreq devList[MAX_DEV];
		       
  struct ifconf ifConf;
  ifConf.ifc_len = MAX_DEV * sizeof(struct ifreq);
  ifConf.ifc_req = devList;

  int ret;

  ret = ioctl(fd, SIOCGIFCONF, &ifConf);
  assert ( ret >= 0);



  for (unsigned i = 0; i < ifConf.ifc_len / sizeof(struct ifreq); ++i) {

    int lev = level;
    if (strstr(devList[i].ifr_name, "wlan")) --lev; /* one point for the name */
//    std::cout << i << ". " << devList[i].ifr_name << std::endl;
	 

    ret = ioctl(fd, SIOCGIFFLAGS, &devList[i]);
    assert ( ret >= 0 );
    
    if (devList[i].ifr_flags & IFF_UP ) --lev;        /* one point if the interface is up */
    if (devList[i].ifr_flags & IFF_BROADCAST ) --lev; /* one point for a broadcast device */
    if (devList[i].ifr_flags & IFF_LOOPBACK ) ++lev;  /* one MINUS point for a loopback device */
    
    ret = ioctl(fd, SIOCGIFADDR, &devList[i]);
    assert ( ret >= 0 );
    
    const char * addr_string = 
      inet_ntoa( ( (struct sockaddr_in *) &(devList[i].ifr_addr) )->sin_addr );
    assert( addr_string );
    
    /* one point for private network id */
    if (strstr(addr_string, "10.") == addr_string) --lev; 
    if (strstr(addr_string, "192.168.") == addr_string) --lev; 
    
    if (lev == 0) {
      strcpy(ip_addr, addr_string);
      close(fd);
      return true;
    }
    
  }
  
  close(fd);
  
  return false;
}

void guess_iface(char *ret) {

  int lvl = 5;

  while( ! guess_iface(lvl--, ret) );
  
}


#endif
