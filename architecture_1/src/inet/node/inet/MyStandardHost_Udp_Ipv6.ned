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

package inet.node.inet;

import inet.node.base.MyApplicationLayerNodeBase_Udp_Ipv6;

//
// IPv6 host with TCP, SCTP and UDP layers and applications.
// see ~StandardHost for configuration.
//
module MyStandardHost_Udp_Ipv6 extends MyApplicationLayerNodeBase_Udp_Ipv6 {
    parameters:
        @display("i=device/pc2");
        xml routes = default(xml("<routingTable/>"));
        ipv6.routingTable.routes = routes;
}