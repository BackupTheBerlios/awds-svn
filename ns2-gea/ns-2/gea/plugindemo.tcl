
set val(chan)           Channel/WirelessChannel    ;#Channel Type
set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type
set val(mac)            Mac/802_11                 ;# MAC type
set val(ifq)            Queue/DropTail/PriQueue    ;# interface queue type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         50                         ;# max packet in ifq
set val(nn)             9                          ;# number of mobilenodes
#set val(rp)             DSDV                       ;# routing protocol
#set val(rp)             DSR                       ;# routing protocol
set val(rp)             AODV                       ;# routing protocol
set val(x)		510
set val(y)		400

# Initialize Global Variables
set ns_		[new Simulator]
set tracefd     [open plugindemo.tr w]
$ns_ trace-all $tracefd

set namtrace [open plugindemo.nam w]
$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)

# set up topography object
set topo       [new Topography]

$topo load_flatgrid $val(x) $val(y)

# Create God
create-god $val(nn)

# Create channel #1 and #2
set chan_1_ [new $val(chan)]

# configure node, please note the change below.
$ns_ node-config -adhocRouting $val(rp) \
		-llType $val(ll) \
		-macType $val(mac) \
		-ifqType $val(ifq) \
		-ifqLen $val(ifqlen) \
		-antType $val(ant) \
		-propType $val(prop) \
		-phyType $val(netif) \
		-topoInstance $topo \
		-agentTrace ON \
		-routerTrace ON \
		-macTrace ON \
		-movementTrace OFF \
		-channel $chan_1_ 

# create nodes
for {set i 0} {$i < $val(nn)} {incr i} {
    set node_($i) [$ns_ node]
}


# place nodes at random positions
# use a zero-move to force nam to show nodes at there positions
#for {set i 0} {$i < $val(nn)} {incr i} {
#    set px [$rng integer $val(x)]
#    set py [$rng integer $val(y)]
#    set pz 0
#    $node_($i) set X_ $px
#    $node_($i) set Y_ $py
#    $node_($i) set Z_ $pz
#    $node_($i) color "red"
#    $ns_ at 0 "$node_($i) setdest $px $py 0"
#    $ns_ at 0 "$node_($i) color green"    
#    $ns_ at 0 "$node_($i) label \"Text with spaces\""        
#}

set px 20
set py 20
set pz 0
$node_(0) set X_ $px
$node_(0) set Y_ $py
$node_(0) set Z_ $pz
$node_(0) color "red"
$ns_ at 0 "$node_(0) setdest $px $py 0"
$ns_ at 0 "$node_(0) color green"    
$ns_ at 0 "$node_(0) label \"euk-lab101\""

set px 170
set py 20
set pz 0
$node_(1) set X_ $px
$node_(1) set Y_ $py
$node_(1) set Z_ $pz
$node_(1) color "red"
$ns_ at 0 "$node_(1) setdest $px $py 0"
$ns_ at 0 "$node_(1) color green"    
$ns_ at 0 "$node_(1) label \"euk-lab102\""

set px 340
set py 20
set pz 0
$node_(2) set X_ $px
$node_(2) set Y_ $py
$node_(2) set Z_ $pz
$node_(2) color "red"
$ns_ at 0 "$node_(2) setdest $px $py 0"
$ns_ at 0 "$node_(2) color green"    
$ns_ at 0 "$node_(2) label \"euk-lab103\""

set px 170
set py 140
set pz 0
$node_(3) set X_ $px
$node_(3) set Y_ $py
$node_(3) set Z_ $pz
$node_(3) color "red"
$ns_ at 0 "$node_(3) setdest $px $py 0"
$ns_ at 0 "$node_(3) color green"    
$ns_ at 0 "$node_(3) label \"euk-lab106\""

set px 340
set py 140
set pz 0
$node_(4) set X_ $px
$node_(4) set Y_ $py
$node_(4) set Z_ $pz
$node_(4) color "red"
$ns_ at 0 "$node_(4) setdest $px $py 0"
$ns_ at 0 "$node_(4) color green"    
$ns_ at 0 "$node_(4) label \"euk-lab105\""

set px 490
set py 140
set pz 0
$node_(5) set X_ $px
$node_(5) set Y_ $py
$node_(5) set Z_ $pz
$node_(5) color "red"
$ns_ at 0 "$node_(5) setdest $px $py 0"
$ns_ at 0 "$node_(5) color green"    
$ns_ at 0 "$node_(5) label \"euk-lab104\""

set px 20
set py 380
set pz 0
$node_(6) set X_ $px
$node_(6) set Y_ $py
$node_(6) set Z_ $pz
$node_(6) color "red"
$ns_ at 0 "$node_(6) setdest $px $py 0"
$ns_ at 0 "$node_(6) color green"    
$ns_ at 0 "$node_(6) label \"euk-lab107\""

set px 170
set py 380
set pz 0
$node_(7) set X_ $px
$node_(7) set Y_ $py
$node_(7) set Z_ $pz
$node_(7) color "red"
$ns_ at 0 "$node_(7) setdest $px $py 0"
$ns_ at 0 "$node_(7) color green"    
$ns_ at 0 "$node_(7) label \"euk-lab108\""

set px 340
set py 380
set pz 0
$node_(8) set X_ $px
$node_(8) set Y_ $py
$node_(8) set Z_ $pz
$node_(8) color "red"
$ns_ at 0 "$node_(8) setdest $px $py 0"
$ns_ at 0 "$node_(8) color green"    
$ns_ at 0 "$node_(8) label \"euk-lab109\""

#new GeaPlugin $node_(0) "./hop.gea" -i 1 2 3 4 5 6 7 8 0
#$ns_ at 0 "$node_(0) start";


# for {set i 1} {$i < $val(nn)} {incr i} {
#     new GeaPlugin $node_($i) "../gea/hop.gea" -s
#     $ns_ at 0 "$node_($i) start";
# }

$ns_ at 0 "$node_(0) gea_start /home/mahrenho/SPP/prog/gea/test/periodic.gea"
$ns_ at 0 "$node_(0) start";

# create node
# disable random motion
# set note size to 20
for {set i 0} {$i < $val(nn)} {incr i} {
    $node_($i) random-motion 0
    $ns_ initial_node_pos $node_($i) 20
}

#
# Tell nodes when the simulation ends
#
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at 100.0 "$node_($i) reset";
}

$ns_ at 100.0 "stop"
$ns_ at 100.01 "puts \"NS EXITING...\" ; $ns_ halt"

proc stop {} {
    global ns_ tracefd
    $ns_ flush-trace
    close $tracefd
}

puts "Starting Simulation..."
$ns_ run
