// src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_ShoreStation.ned

package inet.node.inet;

import inet.node.base.MyNetworkLayerNodeBase_Ipv4_ShoreStation;

import inet.common.MessageDispatcher;
import inet.transportlayer.contract.IUdp;

module MyRouter_Udp_Ipv4_Wireless_ShoreStation extends MyNetworkLayerNodeBase_Ipv4_ShoreStation {
    parameters:
        @display("i=device/accesspoint");
        @figure[submodules];
        forwarding = true;
        numWmtcInterfaces = default(1);
        
    submodules:
        udp: <default(firstAvailableOrEmpty("Udp"))> like IUdp {
            parameters:
                @display("p=663,189.8");
        }
        tn: MessageDispatcher {
            parameters:
                @display("p=663,266.5;b=500,5,,,,1");
        }
    connections allowunconnected:
        udp.ipOut --> tn.in++;
        udp.ipIn <-- tn.out++;

        tn.out++ --> ipv4.transportIn;
        tn.in++ <-- ipv4.transportOut;
}