package inet.node.inet;

import inet.node.base.MyNetworkLayerNodeBase_Ipv6;

module MyRouter_Udp_Ipv6_Wireless extends MyNetworkLayerNodeBase_Ipv6 {
    parameters:
        @display("i=abstract/router");
        @figure[submodules];
        forwarding = true;
        numWlanInterfaces = default(1);
}