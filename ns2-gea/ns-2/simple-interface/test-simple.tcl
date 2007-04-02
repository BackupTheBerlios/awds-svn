source node-common/utils.tcl

set ns [new Simulator]
$ns use-scheduler Heap

set nodeConstructor [new NodeConstructor];
set channel [new TclSimpleBroadcastChannel];
set interfaceConstructor [new TclSimpleNetInterfaceConstructor];

set nodea [$nodeConstructor create-node];
set interface [$interfaceConstructor create-interface];
$nodea add-interface $interface $channel

set nodeb [$nodeConstructor create-node];
set interface [$interfaceConstructor create-interface];
$nodeb add-interface $interface $channel

# dest sink
set sink [new Agent/TCPSink]
$nodea attach-agent $sink
# source 
set source [new Agent/TCP]
$source set class_ 2
$nodeb attach-agent $source
# connect source to dest
ip-connect $source $sink;
# 
set ftp [new Application/FTP]
$ftp attach-agent $source
# start source at 3.0
$ns at 3.0 "$ftp start" 

proc simulation_end {} {
    global ::ns;
    global ::ftp;
    $ftp stop;
    $ns halt;
}


$ns at 3.2 simulation_end
$ns run
