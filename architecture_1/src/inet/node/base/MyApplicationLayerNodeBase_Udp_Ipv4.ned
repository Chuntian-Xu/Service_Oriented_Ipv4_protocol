// src/inet/node/base/MyApplicationLayerNodeBase_Udp_Ipv4.ned

package inet.node.base;

import inet.common.MessageDispatcher;
import inet.applications.contract.IApp;

module MyApplicationLayerNodeBase_Udp_Ipv4 extends MyTransportLayerNodeBase_Udp_Ipv4
{
    parameters:
        int numApps = default(0);
        @figure[applicationLayer](type=rectangle; pos=250,6; size=1000,130; lineColor=#808080; cornerRadius=5; fillColor=#ffff00; fillOpacity=0.1);
        @figure[applicationLayer.title](type=text; pos=1245,11; anchor=ne; text="application layer");

    submodules:
        UdpApp[numApps]: <default("UdpBasicApp")> like IApp {  
            parameters:
                @display("p=663,39,row,150");
        }
        at: MessageDispatcher {
            parameters:
                @display("p=663,115.7;b=500,5,,,,1");
        }

    connections allowunconnected:
        for i=0..numApps-1 {
            UdpApp[i].socketOut --> at.in++;
            UdpApp[i].socketIn <-- at.out++;
        }

        at.out++ --> udp.appIn;
        at.in++ <-- udp.appOut;
}
