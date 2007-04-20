

#include <algorithm>

#include <awds/Topology.h>
#include <awds/TopoPacket.h>
#include <awds/routing.h>
#include <awds/RateMonitor.h>

#include <awds/Metric.h>

#include <iostream>

using namespace std;

void TopoPacket::setNeigh(AwdsRouting *awdsRouting, gea::AbsTime t) {
  
    unsigned char  n = 0;
	    
    char *addr =  &(packet.buffer[OffsetLinks]);

    /*    
	  RateMonitor *rm = awdsRouting->madwifiRateMonitor;
    
	  if ( (awdsRouting->metrics == Routing::TransmitDurationMetrics) && (rm != 0) )
	  rm->update();*/

    awdsRouting->topology->metric->update();
    for (int i = 0; i < awdsRouting->numNeigh; ++i) {
	    
	if ( awdsRouting->neighbors[i].isBidiGood(t, awdsRouting->myNodeId) )
	    {
		NodeId nId = awdsRouting->neighbors[i].id;
		nId.toArray(addr); 
		++n;
		addr += NodeId::size;
		// the installed metric decides which quality values will be send
		RTopology::link_quality_t quality = awdsRouting->topology->metric->get_quality(awdsRouting->neighbors[i]);
		
		/*		switch (awdsRouting->metrics) {
		  
		case Routing::PacketLossMetrics:
		metric = 0xff - (awdsRouting->neighbors[i].quality() - 1) * (0x100 / 32);    
		break;
		
		case Routing::EtxMetrics: 
		metric = 0xff - (awdsRouting->neighbors[i].quality() - 1) * (0x100 / 32);
		break;
		
		case Routing::TransmitDurationMetrics:
		{
		assert(rm);
		rm->update();
		metric = rm->getTT(nId) / 256;
		}
		break;
		case Routing::HopCountMetrics:
		metric = 0xff;
		break;
		}
		
		// clamp ranges dass machen wir dann auch in der metric
		metric = max( min(metric, 0xff), 1);
		*/
		
		*(reinterpret_cast<RTopology::link_quality_t*>(addr)) = quality;
		assert(*(reinterpret_cast<RTopology::link_quality_t*>(addr)));
		addr += sizeof(RTopology::link_quality_t);

	    }
    }	
    const char* nodeName = awdsRouting->topology->nodeName;
    int len = strlen(nodeName);
    if (len > 32) len = 32;
    *(addr++) = (char)len;
    memcpy(addr, nodeName, len);
    
    packet.buffer[OffsetNumLinks] = (char)n;
    packet.size = 
	OffsetLinks + (NodeId::size + 1) * (size_t)(n*sizeof(RTopology::link_quality_t)) 
	+ len+1 /* the station name */;
    
    //  assert(getNumLinks() == n);
}   

/* magic cookie */
const char *magic = "\t\r\naw"/*""*/"ds routi"
    "n"/*"Hello Welt's"*/"g "
    /**/"�""2""0""0""6 by A";
const char *magic2 = "ndr"
    "�"
    " "
    "H" /*0x100, 0x23, 0x42, 0x54, 0x00*/
    "e""r""m""s ";

#include <iostream>

using namespace std;

void TopoPacket::print() {

    ostream& os = GEA.dbg();
    
    os << "packet[" << packet.size << "] " << getSrc() << " : ";
    
    int n = getNumLinks();
    
    char *addr =  &(packet.buffer[OffsetLinks]);
    
    NodeId id;
    for (int i = 1 ; i < n; ++i) {
	
	id.fromArray(addr);
	os << id << " ";
	addr += NodeId::size + 1;
    }
    
    os << endl;
}


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
