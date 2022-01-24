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

import inet.applications.contract.IApp;
import inet.common.MessageDispatcher;

module MyApplicationLayerNodeBase_Udp_Ipv6 extends MyTransportLayerNodeBase_Udp_Ipv6
{
    parameters:
        int numApps = default(0);
        @figure[applicationLayer](type=rectangle; pos=250,6; size=1000,130; lineColor=#808080; cornerRadius=5; fillColor=#ffff00; fillOpacity=0.1);
        @figure[applicationLayer.title](type=text; pos=1245,11; anchor=ne; text="application layer");

    submodules:
        UdpApp[numApps]: <default("UdpBasicApp")> like IApp {  //如果没有指定，则默认App类型为UdpBasicApp
            parameters:
                @display("p=670.8,35.1,row,150");
        }
        at: MessageDispatcher {
            parameters:
                @display("p=673.39996,127.4;b=500,5,,,,1");
        }

    connections allowunconnected:
        for i=0..numApps-1 {
            UdpApp[i].socketOut --> at.in++;
            UdpApp[i].socketIn <-- at.out++;
        }

        at.out++ --> udp.appIn;
        at.in++ <-- udp.appOut;
}
