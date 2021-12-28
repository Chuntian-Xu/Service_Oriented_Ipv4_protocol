// src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_Station.ned

package inet.node.inet;

import inet.node.base.MyNetworkLayerNodeBase_Ipv4_Service;

module MyRouter_Udp_Ipv4_Wireless_Station extends MyNetworkLayerNodeBase_Ipv4_Service {
    parameters:
        @display("i=abstract/router");
        @figure[submodules];
        forwarding = true;
        numWlanInterfaces = default(1);
}