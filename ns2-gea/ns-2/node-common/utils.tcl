proc ip-connect {source dest} {
    $source set dst_addr_ [$dest set agent_addr_]
    $source set dst_port_ [$dest set agent_port_]
    $dest set dst_addr_ [$source set agent_addr_]
    $dest set dst_port_ [$source set agent_port_]
}
