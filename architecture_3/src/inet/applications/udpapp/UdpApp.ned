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

package inet.applications.udpapp;

import inet.applications.contract.IApp;
import inet.queueing.contract.IPassivePacketSink;
import inet.queueing.contract.IActivePacketSource;

import inet.queueing.contract.IPassivePacketSource;

//
// This module generates traffic for a UDP application. The traffic source and
// traffic sink modules can be built from queueing model elements.
//
module UdpApp like IApp
{
    parameters:
        string interfaceTableModule;
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        @display("i=block/app");
    gates:
        input socketIn;
        output socketOut;
    submodules:
        //        source: <default("ActivePacketSource")> like IActivePacketSource {
        //            parameters:
        //                @display("p=100,100");
        //        }
        //        sink: <default("PassivePacketSink")> like IPassivePacketSink {
        //            parameters:
        //                @display("p=200,100");
        //        }

        source: <default("PassivePacketSource")> like IPassivePacketSource {
            parameters:
                @display("p=73,100");
        }
        sink: <default("PassivePacketSink")> like IPassivePacketSink {
            parameters:
                @display("p=234,100");
        }
        io: UdpSocketIo {
            parameters:
                @display("p=150,197");
        }

    connections:
        source.out --> io.trafficIn;
        io.socketOut --> socketOut;
        socketIn --> io.socketIn;
        io.trafficOut --> sink.in;
}
