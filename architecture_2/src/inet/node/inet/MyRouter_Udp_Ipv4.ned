package inet.node.inet;

import inet.node.base.MyNetworkLayerNodeBase_Ipv4;

module MyRouter_Udp_Ipv4 extends MyNetworkLayerNodeBase_Ipv4 {
    parameters:
        @display("i=abstract/router");
        @figure[submodules];
        forwarding = true;
}