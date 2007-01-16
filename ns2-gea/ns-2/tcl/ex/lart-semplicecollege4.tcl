######################################################################
# Parametri di configurazione #
######################################################################

set val(chan)	Channel/WirelessChannel
set val(prop)	Propagation/TwoRayGround
set val(ant)	Antenna/OmniAntenna
set val(ll)	LL
set val(ifq)	Queue/DropTail/PriQueue
set val(ifqlen)	50
set val(netif)	Phy/WirelessPhy
set val(mac)	Mac/802_11
set val(rp)	DSDV
set val(ahrp)	DSDV

set val(x)	670
set val(y)	300

set val(stop)	200

set num_wired_nodes	3
set num_bs_nodes	2
set val(nn)		1

set chan_1	[new $val(chan)]
#set chan_1	[new TclFreeSpaceBroadcastChannel]
#set channel $chan_1
#set nodeConstructor [new NodeConstructor];
#set interfaceConstructor [new TclNetInterfaceConstructor80211];
########################################################################

set ns		[new Simulator]
$ns use-scheduler Heap

########################################################################
# Topology #
########################################################################

set topo	[new Topography]
$topo load_flatgrid $val(x) $val(y)
set god 	[create-god [expr $num_bs_nodes + $val(nn)]]

########################################################################
# Setting Domain and Clusters #
########################################################################

$ns node-config -addressType hierarchical

AddrParams set domain_num_ [expr $num_bs_nodes + 1]

lappend cluster_num 2 1 1
AddrParams set cluster_num_ $cluster_num

lappend eilastlevel 1 2 1 [expr $val(nn) + 1]
AddrParams set nodes_num_ $eilastlevel

########################################################################
# open output file #
########################################################################

#set tracefd [open college.tr w]

set tracefd [open college.tr w]
set namtrace [open college.nam w]
$ns trace-all $tracefd
$ns namtrace-all-wireless $namtrace $val(x) $val(y)

########################################################################
# create network
########################################################################

# wired
set temp {0.0.0 0.1.0 0.1.1}
set distance 100
for {set i 0} {$i < $num_wired_nodes} {incr i} {
	set W_($i) [$ns node [lindex $temp $i]]

#	$W_($i) set X_ [expr $distance * [expr $i+1]].0
#	$W_($i) set Y_ 50.0
#	$W_($i) set Z_ 0.0
}
#for {set i 0} {$i < $num_wired_nodes} {incr i} {
#	$ns initial_node_pos $W_($i) 10
#}

# bs wireless
$ns node-config -adhocRouting	$val(ahrp) \
		-mobileIP	ON \
		-llType		$val(ll) \
		-macType	$val(mac) \
		-ifqType	$val(ifq) \
		-ifqLen		$val(ifqlen) \
		-antType	$val(ant) \
		-propType	$val(prop) \
		-phyType	$val(netif) \
		-channel 	$chan_1 \
		-topoInstance	$topo \
		-wiredRouting 	ON \
		-agentTrace 	ON \
		-routerTrace 	OFF \
		-macTrace	ON \
		-isAp		ON \
		-movementTrace 	OFF  

set temp {1.0.0 2.0.0 3.0.0 4.0.0 5.0.0}
set distance 200
#$interfaceConstructor set-nqap
for {set j 0} {$j < $num_bs_nodes} {incr j} {
	set BS_($j) [$ns node [lindex $temp $j]]
	$BS_($j) random-motion 0
	$BS_($j) set X_ [expr $distance * [expr $j+1]].0
	$BS_($j) set Y_ 100.0
	$BS_($j) set Z_ 0.0
#	set macBS_($j) [$BS_($j) getMac 0]
#	$macBS_($j) bss_id [$macBS_($j) id]
#	set BS_($j) [$nodeConstructor create-node]
#	$BS_($j) set-position [expr $distance * [expr $j+1]].0 100.0 0.0
#	$BS_($j) add-interface [$interfaceConstructor create-interface] $channel
}

# wireless sta mobile node
$ns node-config -wiredRouting 	OFF \
		-isAp		OFF
	
set temp {1.0.1}
#$interfaceConstructor set-nqsta 1;
for {set z 0} {$z < $val(nn)} {incr z} {
	set node_($z) [$ns node [lindex $temp $z]]
	set HAaddress [AddrParams addr2id [$BS_(0) node-addr]]
	[$node_($z) set regagent_] set home_agent_ $HAaddress
	$node_($z) base-station [AddrParams addr2id [$BS_(0) node-addr]]
	$node_($z) random-motion 0
	$node_($z) set X_ 10.0
	$node_($z) set Y_ 150.0
	$node_($z) set Z_ 0.0
#	set macnode_($z) [$node_($z) getMac 0]
#	$macnode_($z) bss_id [$macBS_(0) id]
#	$node_($z) set isAp_ 0
#	set node_($z) [$nodeConstructor create-node]
#	$node_($z) set-position 10.0 100.0 0.0
#	$node_($z) add-interface [$interfaceConstructor create-interface] $channel
}

#########################################################################
# create link and traffic #
#########################################################################


$ns duplex-link $W_(0) $W_(1) 50Mb 2ms DropTail
$ns duplex-link $W_(0) $W_(2) 50Mb 2ms DropTail
$ns duplex-link $W_(1) $W_(2) 50Mb 2ms DropTail

$ns duplex-link $W_(1) $BS_(0) 50Mb 2ms DropTail
$ns duplex-link $W_(2) $BS_(1) 50Mb 2ms DropTail

$ns duplex-link-op $W_(0) $W_(1) orient up-left
$ns duplex-link-op $W_(0) $W_(2) orient up-right

$ns duplex-link-op $W_(1) $BS_(0) orient up-left
$ns duplex-link-op $W_(2) $BS_(1) orient up

set udp [new Agent/UDP]
set null [new Agent/Null]
$ns attach-agent $node_(0) $udp
$ns attach-agent $W_(0) $null
$ns connect $udp $null

set cbr [new Application/Traffic/CBR]
#$cbr set packetSize_ 500
#$cbr set interval_ 0.005
$cbr attach-agent $udp

#$ns at 10.0 "$cbr start"
#$ns at 199.0 "$cbr stop"

proc finish {} {
	global ns tracefd namtrace
	$ns flush-trace
	close $tracefd
	close $namtrace
#	exec nam college.nam &
#	exit 0
}
$ns at 5.0 "$node_(0) setdest 416.00000000 150.00000000 10.00000000"



$ns at $val(stop).0001 "finish"
$ns at $val(stop).0002 "puts \"NS EXITING...\" ; $ns halt"


#for {set j 0} {$j < $num_bs_nodes} {incr j} {
#	$ns initial_node_pos $BS_($j) 5
#}
for {set i 0} {$i < $val(nn)} {incr i} {
	$ns initial_node_pos $node_($i) 20
}

#for {set i 0} {$i < $num_wired_nodes} {incr i} {
#	$ns at $val(stop).0 "$W_($i) reset"
#}
for {set i 0} {$i < $num_bs_nodes} {incr i} {
	$ns at $val(stop).0 "$BS_($i) reset";
}
for {set i 0} {$i < $val(nn)} {incr i} {
	$ns at $val(stop).0 "$node_($i) reset";
}

$ns run
