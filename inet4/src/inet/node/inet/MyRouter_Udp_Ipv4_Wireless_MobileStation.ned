// src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_MobileStation.ned

package inet.node.inet;

import inet.node.base.MyNetworkLayerNodeBase_Ipv4_MobileStation;

module MyRouter_Udp_Ipv4_Wireless_MobileStation extends MyNetworkLayerNodeBase_Ipv4_MobileStation {
    parameters:
        @display("i=abstract/router");
        @figure[submodules];
        forwarding = true;
        numWlanInterfaces = default(1);
}