// src/inet/node/base/MyTransportLayerNodeBase_Udp_Ipv4.ned 

package inet.node.base;

import inet.common.MessageDispatcher;
import inet.transportlayer.contract.IUdp;

module MyTransportLayerNodeBase_Udp_Ipv4 extends MyNetworkLayerNodeBase_Ipv4
{
    parameters:
        @figure[transportLayer](type=rectangle; pos=250,156; size=1000,130; fillColor=#ff0000; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[transportLayer.title](type=text; pos=1245,161; anchor=ne; text="transport layer");
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
