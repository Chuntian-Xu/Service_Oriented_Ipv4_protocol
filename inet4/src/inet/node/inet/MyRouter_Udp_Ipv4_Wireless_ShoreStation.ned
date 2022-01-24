// src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_ShoreStation.ned 

package inet.node.inet;

import inet.node.base.MyNetworkLayerNodeBase_Ipv4_ShoreStation;

module MyRouter_Udp_Ipv4_Wireless_ShoreStation extends MyNetworkLayerNodeBase_Ipv4_ShoreStation {
    parameters:
        @display("i=abstract/router");
        @figure[submodules];
        forwarding = true;
        numWmtcInterfaces = default(1);
}