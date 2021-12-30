// src/inet/node/base/MyNetworkLayerNodeBase_Ipv4_MobileStation.ned

package inet.node.base;

import inet.common.MessageDispatcher;
import inet.networklayer.contract.INetworkLayer;  

module MyNetworkLayerNodeBase_Ipv4_MobileStation extends LinkLayerNodeBase {
    parameters:
        bool forwarding = default(false);
        bool multicastForwarding = default(false);
        *.forwarding = forwarding;  
        *.multicastForwarding = multicastForwarding;
        @figure[networkLayer](type=rectangle; pos=250,306; size=1000,130; fillColor=#00ff00; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[networkLayer.title](type=text; pos=1245,311; anchor=ne; text="network layer");
    submodules:
        ipv4: <default("Ipv4NetworkLayerMobileStation")> like INetworkLayer {  // Ipv4NetworkLayerMobileStation module  // new added
            parameters:
                @display("p=663,341.9;q=queue");
        }
        nl: MessageDispatcher {
            parameters:
                @display("p=663,414.69998;b=500,5,,,,1");
        }
    connections allowunconnected:
        ipv4.ifIn <-- nl.out++;
        ipv4.ifOut --> nl.in++;

        for i=0..numLoInterfaces-1 {
            nl.out++ --> lo[i].upperLayerIn;
            lo[i].upperLayerOut --> nl.in++;
        }
        for i=0..sizeof(radioIn)-1 {
            wlan[i].upperLayerOut --> nl.in++;
            wlan[i].upperLayerIn <-- nl.out++;
        }
        for i=0..sizeof(ethg)-1 {
            eth[i].upperLayerOut --> nl.in++;
            eth[i].upperLayerIn <-- nl.out++;
        }
        for i=0..sizeof(pppg)-1 {
            ppp[i].upperLayerOut --> nl.in++;
            ppp[i].upperLayerIn <-- nl.out++;
        }
        for i=0..numTunInterfaces-1 {
            tun[i].upperLayerOut --> nl.in++;
            tun[i].upperLayerIn <-- nl.out++;
        }
        for i=0..numVlanInterfaces-1 {
            vlan[i].upperLayerOut --> nl.in++;
            vlan[i].upperLayerIn <-- nl.out++;
        }
}

