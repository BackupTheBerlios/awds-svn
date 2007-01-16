#Copyright (c) 1997 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#      This product includes software developed by the Computer Systems
#      Engineering Group at Lawrence Berkeley Laboratory.
# 4. Neither the name of the University nor of the Laboratory may be used
#    to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $Header: /nfs/jade/vint/CVSROOT/ns-2/tcl/ex/wireless-mitf.tcl,v 1.2 2000/08/30 00:10:45 haoboy Exp $
#
# Simple demo script for the new APIs to support multi-interface for 
# wireless node.
#
# Define options
# Please note: 
# 1. you can still specify "channelType" in node-config right now:
# set val(chan)           Channel/WirelessChannel
# $ns_ node-config ...
#		 -channelType $val(chan)
#                  ...
# But we recommend you to use node-config in the way shown in this script
# for your future simulations.  
# 
# 2. Because the ad-hoc routing agents do not support multiple interfaces
#    currently, this script can't generate anything interesting if you config
#    the interfaces of node 1 and 2 on different channels
#   
#     --Xuan Chen, USC/ISI, July 21, 2000
#
set val(chan)           Channel/WirelessChannel    ;#Channel Type
#set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(prop)           Propagation/Ricean   	   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type
#set val(mac)            Mac/802_11                 ;# MAC type
set val(mac)            Mac                 ;# MAC type
set val(ifq)            Queue/DropTail/PriQueue    ;# interface queue type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         50                         ;# max packet in ifq
set val(nn)             2                          ;# number of mobilenodes
#set val(rp)             TORA                       ;# routing protocol
set val(rp)             NORT                       ;# routing protocol
set val(x)		1500
set val(y)		1500

set val(endtime)	500
# Initialize Global Variables
set ns_		[new Simulator]
set tracefd     [open rjp-sample.tr w]
$ns_ trace-all $tracefd

#set namtrace [open wireless_mitf.nam w]
#$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)

# set up topography object
set topo       [new Topography]

$topo load_flatgrid $val(x) $val(y)

# Create God
create-god $val(nn)

#set god [new God]
#$god num_nodes $val(nn)
#$god set-dist 0 1 1

# New API to config node: 
# 1. Create channel (or multiple-channels);
# 2. Specify channel in node-config (instead of channelType);
# 3. Create nodes for simulations.

# Create channel #1 and #2
set chan_1_ [new $val(chan)]
set chan_2_ [new $val(chan)]

# Create node(0) "attached" to channel #1

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
		-agentTrace OFF \
		-routerTrace OFF \
		-macTrace ON \
		-movementTrace OFF \
		-channel $chan_1_ 

set prop_inst [$ns_ set propInstance_]
$prop_inst MaxVelocity  2.5;
$prop_inst RiceanK        6;
$prop_inst LoadRiceFile  "rice_table.txt";


###############################################
#   LOG the propagation information
#  To disable logging, simply comment this section
#  out
## ###########################################
set prop_tracefd [open rjp-proptrace.tr w];
set prop_log [new Trace/Generic]
$prop_log target [$ns_ set nullAgent_]
$prop_log attach $prop_tracefd
$prop_log set src_ 0
$prop_inst tracetarget $prop_log
##############################################

set node_(0) [$ns_ node]

# node_(1) can also be created with the same configuration, or with a different
# channel specified.
# Uncomment below two lines will create node_(1) with a different channel.
#  $ns_ node-config \
#		 -channel $chan_2_ 
set node_(1) [$ns_ node]

$node_(0) random-motion 0
$node_(1) random-motion 0

for {set i 0} {$i < $val(nn)} {incr i} {
	$ns_ initial_node_pos $node_($i) 20
}

#
# Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
#
#
# Now produce some simple node movements
$node_(0) set X_    10.00
$node_(0) set Y_    10.00
$node_(0) set Z_     0.00

$node_(1) set X_     100.00
$node_(1) set Y_   100.00
$node_(1) set Z_     0.00
#$god_ set-dist 1 2 1
$ns_ at 0.000000000000 "$node_(1) setdest 1200.00 1200.00 5.0"

# Setup traffic flow between nodes
# TCP connections between node_(0) and node_(1)

#    set tcp [new Agent/TCP]
#    $tcp set class_ 2
#    set sink [new Agent/TCPSink]
#    $ns_ attach-agent $node_(0) $tcp
#    $ns_ attach-agent $node_(1) $sink
#    $ns_ connect $tcp $sink
#    set ftp [new Application/FTP]
#    $ftp attach-agent $tcp
#    $ns_ at 3.0 "$ftp start" 

#set cbr_(0) [new Agent/Traffic/CBR]
set cbr_(0) [$ns_ create-connection  CBR $node_(0) CBR $node_(1) 0]
$cbr_(0) set packetSize_ 1024
$cbr_(0) set interval_ 0.2
$cbr_(0) set random_ 1
$cbr_(0) set maxpkts_ 10000
$ns_ at 10.000 "$cbr_(0) start"



#
# Tell nodes when the simulation ends
#
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(endtime) "$node_($i) reset";
}
$ns_ at $val(endtime) "stop"
$ns_ at $val(endtime) "puts \"NS EXITING...\" ; $ns_ halt"
proc stop {} {
    global ns_ tracefd
    $ns_ flush-trace
    close $tracefd
}

puts "Starting Simulation..."
$ns_ run
