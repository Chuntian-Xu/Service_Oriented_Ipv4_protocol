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
import inet.transportlayer.contract.IUdp;

module MyTransportLayerNodeBase_Udp_Ipv6 extends MyNetworkLayerNodeBase_Ipv6
{
    parameters:
        @figure[transportLayer](type=rectangle; pos=250,156; size=1000,130; fillColor=#ff0000; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[transportLayer.title](type=text; pos=1245,161; anchor=ne; text="transport layer");
    submodules:
        udp: <default(firstAvailableOrEmpty("Udp"))> like IUdp {
            parameters:
                @display("p=670.8,210.59999");
        }
        tn: MessageDispatcher {
            parameters:
                @display("p=670.8,283.4;b=500,5,,,,1");
        }
    connections allowunconnected:
        udp.ipOut --> tn.in++;
        udp.ipIn <-- tn.out++;

        tn.out++ --> ipv6.transportIn;
        tn.in++ <-- ipv6.transportOut;
}
