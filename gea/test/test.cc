#include "gea/Time.h"

#include "gea/API.h"
#include "gea/posix/ShadowEventHandler.h"
#include "gea/posix/UnixFdHandle.h"
#include "gea/Blocker.h"
#include "gea/UdpHandle.h"

#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cassert>
#include <string>
#include <vector>

#define MAX_ADDR_NUM 10

using namespace std;
using namespace gea;

struct packet_fmt
{
  __uint16_t num_addr;
  __uint16_t cur_addr;
  __uint8_t addr_list[4*MAX_ADDR_NUM];
  __uint16_t port_list[MAX_ADDR_NUM];
  __uint32_t seq_num;
  __uint8_t dummy_data[1000];
} __attribute__ ((packed));

struct packet_fmt roundpkt, // filled by do_getopt
		   sendpkt; // transmitted

AbsTime t0 = AbsTime::now();
AbsTime init = AbsTime::now();
AbsTime rtt = AbsTime::now();

unsigned long g_seq_num = (unsigned long)-1;
unsigned long last_rtt = 0;

EventHandler eh;

Blocker blocker;

UdpHandle hSend(UdpHandle::Write, UdpAddress(1000, "127.0.0.1"));
UdpHandle hRecv(UdpHandle::Read, UdpAddress(1000, "127.0.0.1"));

char *Station_Addr_String;
int Station_Port_Num;
bool stationMode = true;


/**************************************************************************************************/
void print_usage(const char * const *argv) {	// Print Usage Info
  cout << "Usage: " << argv[0] << " [-s | -i] [IP1:Port1 IP2:Port2 ...]" << endl;
  cout << "-s means station mode, station IP and port are needed" << endl;
  cout << "-i means initiator mode, specify a spaced list of curr. max. " << MAX_ADDR_NUM << " IP Adresses" << endl;
  cout << "Last IP:Port must be the address and port of the initiating station" << endl;
}

// seperate ips from ports given by the command line
void fsplit(const std::string line, const char *sep, std::vector<std::string> &words) {

  std::string::size_type a = 0,e;

  while((a = line.find_first_not_of(sep, a)) != std::string::npos) {
    e = line.find_first_of(sep, a);
    if(e != std::string::npos) {
      words.push_back(line.substr(a, e-a));
      a = e + 1;
    }
    else {
      words.push_back(line.substr(a));
      break;
    }
  }
}

// evaluate command line arguments
void do_getopt(int argc, const char * const *argv) {	// Extract Station Mode and List of IPs from Args
    
  int a = 2;	// argument counter
  struct in_addr *tmp_bin_addr = (struct in_addr *)malloc(sizeof(struct in_addr));
  char *tmp_addr_str;
  int tmp_port;

  // needed to run fsplit
  std::vector<std::string> addr_sep_port;	// vector that stores separated addresses and ports
  std::string addr_port, tmpstdstring;		// string that stores "IP:port" i.e. "127.0.0.1:1000"
  std::vector<std::string>::iterator j;		//

  if(argc == 1) { // no command line option given
    print_usage(argv);
    exit(EXIT_FAILURE);
  }
  if(strcmp(argv[1], "-s") == 0) { // we will run in station mode
    addr_sep_port.clear();
    addr_port = argv[2];
    fsplit(addr_port, ":", addr_sep_port);
    j = addr_sep_port.begin();
    tmpstdstring = *j;
    Station_Addr_String = (char *)tmpstdstring.c_str();
    j++;
    if(j == addr_sep_port.end()) {
      cout << "DO_GETOPT -- Seems you did not specify the station port!" << endl;
      exit(EXIT_FAILURE);
    }
    tmpstdstring = *j;
    Station_Port_Num = atoi((char *)tmpstdstring.c_str());
    stationMode = true;
    return;
  }
  if(strcmp(argv[1], "-i") == 0) stationMode = false; // run as initiator
  else {
    print_usage(argv);
    exit(EXIT_FAILURE);
  }
  if(!stationMode && (argc<=2)) {
    cout << "DO_GETOPT -- No IP List specified! terminating" << endl;
    exit(EXIT_FAILURE);
  }
    
  roundpkt.num_addr = argc-2;	// number of given ip:port arguments
  roundpkt.cur_addr = 0;	// for initiating station point to first IP in list
  while (a < argc) {		// extract IPs and ports
    addr_sep_port.clear();
    addr_port = argv[a];
    fsplit(addr_port, ":", addr_sep_port);
    j = addr_sep_port.begin();
    tmpstdstring = *j;
    tmp_addr_str = (char *)tmpstdstring.c_str();
    j++;
    if(j == addr_sep_port.end()) {
      cout << "DO_GETOPT -- Seems you did not specify some ports!" << endl;
      exit(EXIT_FAILURE);
    }
    tmpstdstring = *j;
    tmp_port = atoi((char *)tmpstdstring.c_str());
    if(inet_aton(tmp_addr_str, tmp_bin_addr) == 0) {
      cout << "DO_GETOPT -- enterd IP address number " << a-2 << " seems to be invalid" << endl;
      cout << "DO_GETOPT -- terminating" << endl;
      exit(EXIT_FAILURE);
    }
    memcpy(&roundpkt.addr_list[(a-2)*4], tmp_bin_addr, 4);
    roundpkt.port_list[(a-2)] = tmp_port;
    a++;
  }
};
/**************************************************************************************************/

// procedure to send udp packets
void udp_send(Handle *h, AbsTime t, void *data) {

  if (h->status == Handle::Ready) {
    h->write(static_cast<char *>(data), sizeof(struct packet_fmt));
//    cout << "UDP_SEND -- Packet sent!" <<endl;
  } 
  else cout << "UDP_SEND -- cannot send: status = " << h->status << endl;
}

// prepare initial packets for transmission
void xmit_initial(Handle *h, AbsTime t, void *data) {

  char *addr_string;
  struct in_addr temp;

  memcpy(&temp, &roundpkt.addr_list[0], 4);
  addr_string = inet_ntoa(temp);
  {
    UdpAddress dest(roundpkt.port_list[0], addr_string);
    hSend.setDest(dest);
  }
  g_seq_num++;
  roundpkt.seq_num = g_seq_num;
  init = AbsTime::now();
  cout << "XMIT_INITIAL -- At t0 + " << (t-t0) << " Sending initial packet to " << addr_string << ":" << roundpkt.port_list[0] << endl;
  eh.waitFor(static_cast<UdpHandle *>(data), t + Duration(.1), udp_send, &roundpkt);
  eh.waitFor(&blocker, t + Duration(.5), xmit_initial, data);
}

// event to receive packets
void recv_event(Handle *h, AbsTime t, void *data) {

  char recv_buf[sizeof(struct packet_fmt)];
  struct packet_fmt fmt_buf;
  struct in_addr temp;
  char *next_addr_str;

  memset(&recv_buf, 0, sizeof(struct packet_fmt));
  memset(&sendpkt, 0, sizeof(struct packet_fmt));
  memset(&fmt_buf, 0, sizeof(struct packet_fmt));

  if(h->status == Handle::Ready) {
    int s = h->read(recv_buf, sizeof(struct packet_fmt));
    if (s >= 0) {

      std::cout << "RECV_EVENT -- recv from ";
      std::cout << (static_cast<gea::UdpHandle *>(h)->getSrc());

      rtt = AbsTime::now();

      memcpy(&fmt_buf, &recv_buf, sizeof(struct packet_fmt));
      cout << " sequence number is " << fmt_buf.seq_num << endl;
      if(((fmt_buf.cur_addr + 1) == fmt_buf.num_addr) && (stationMode == false)) {
        cout << "RECV_EVENT -- At t0 + " << (t-t0) << " Roundtrip done!" << endl;
	{
	  int j = 0;
	  if((j = fmt_buf.seq_num - last_rtt) > 1) cout << j << " packets lost!" << endl;
	}
	last_rtt = fmt_buf.seq_num;
	cout << "RTT is " << (rtt-init) << endl;
//        fmt_buf.cur_addr = 0;
//        memcpy(&temp, &fmt_buf.addr_list[0], 4);
//        next_addr_str = inet_ntoa(temp);
//        {
//          UdpAddress dest(fmt_buf.port_list[0], next_addr_str);
//          hSend.setDest(dest);
//        }
      }
      else {
        fmt_buf.cur_addr++;
        memcpy(&temp, &fmt_buf.addr_list[(fmt_buf.cur_addr)*4], 4);
        next_addr_str = inet_ntoa(temp);
        {
          UdpAddress dest(fmt_buf.port_list[fmt_buf.cur_addr], next_addr_str);
          hSend.setDest(dest);
	  cout << "RECV_EVENT -- Sending packet to next station: " << next_addr_str << ":" << fmt_buf.port_list[fmt_buf.cur_addr] << endl;
        }
//        cout << "RECV_EVENT -- sending received packet" << endl;
        memcpy(&sendpkt, &fmt_buf, sizeof(struct packet_fmt));
        eh.waitFor(&hSend, t + Duration(.1), udp_send, &sendpkt);
      }
    } else std::cout << "RECV_EVENT -- recv error!" << std::endl;
  } else std::cout << "RECV_EVENT -- recv timed out!" << std::endl;
    eh.waitFor(h, t+Duration(10.), recv_event, data);
}

void test_io() {

  if(stationMode == false) {
    {
      struct in_addr tmp_bin_addr;
      memcpy(&tmp_bin_addr, &roundpkt.addr_list[(roundpkt.num_addr - 1)*4], 4);
      UdpAddress addr(roundpkt.port_list[roundpkt.num_addr - 1], inet_ntoa(tmp_bin_addr));
      cout << "TEST_IO -- binding to " << inet_ntoa(tmp_bin_addr) << ":" << roundpkt.port_list[roundpkt.num_addr - 1] << endl;
      hSend.setDest(addr);
      hRecv.setSrc(addr);
    }
    eh.waitFor(&blocker, AbsTime::now() + Duration(.5), xmit_initial, &hSend);
    eh.waitFor(&hRecv, AbsTime::now() + Duration(10.), recv_event, 0);
  }
  
  if(stationMode == true) {
    {
      UdpAddress addr(Station_Port_Num, Station_Addr_String);
      cout << "TEST_IO -- binding to " << Station_Addr_String << ":" << Station_Port_Num << endl;
      hSend.setDest(addr);
      hRecv.setSrc(addr);
    }
    eh.waitFor(&hRecv, AbsTime::now() + Duration(10.), recv_event, 0);
  }
  
      eh.shadow->run();

  
}

extern "C"
int gea_main(int argc, const char * const *argv) {
    
    do_getopt(argc, argv);
    cout << (stationMode?"MAIN -- station":"MAIN -- initiator") << endl;
    test_io();

    return 0;
}

int main(int argc, char **argv) {
  
  int ret = gea_main(argc, argv);
  
  GEA.shadow->run();
  return ret;
}
