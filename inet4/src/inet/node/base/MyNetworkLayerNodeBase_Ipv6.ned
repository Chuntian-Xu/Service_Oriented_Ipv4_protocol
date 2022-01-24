// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.node.base;

import inet.common.MessageDispatcher;
import inet.networklayer.contract.INetworkLayer;

module MyNetworkLayerNodeBase_Ipv6 extends LinkLayerNodeBase {
    parameters:
        bool forwarding = default(false);
        bool multicastForwarding = default(false);
        *.forwarding = forwarding;
        *.multicastForwarding = multicastForwarding;
        @figure[networkLayer](type=rectangle; pos=250,306; size=1000,130; fillColor=#00ff00; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[networkLayer.title](type=text; pos=1245,311; anchor=ne; text="network layer");

    submodules:
        ipv6: <default("Ipv6NetworkLayer")> like INetworkLayer {
            parameters:
                @display("p=670.94995,363.82498;q=queue");
        }
        nl: MessageDispatcher {
            parameters:
                @display("p=670.94995,445.72498;b=500,4,,,,1");
        }
    connections allowunconnected:
        ipv6.ifIn <-- nl.out++;
        ipv6.ifOut --> nl.in++;
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
