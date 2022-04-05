package inet.applications.udpapp;

import inet.applications.contract.IApp;
import inet.queueing.contract.IPassivePacketSink;
import inet.queueing.contract.IActivePacketSource;

module UdpApp_1 like IApp
{
    parameters:
        string interfaceTableModule;
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        @display("i=block/app");
    gates:
        input socketIn;
        output socketOut;
    submodules:
                source: <default("ActivePacketSource")> like IActivePacketSource {
                    parameters:
                        @display("p=100,100");
                }
                sink: <default("PassivePacketSink")> like IPassivePacketSink {
                    parameters:
                        @display("p=200,100");
                }

        io: UdpSocketIo_1 {
            parameters:
                @display("p=150,197");
        }

    connections:
        source.out --> io.trafficIn;
        io.socketOut --> socketOut;
        socketIn --> io.socketIn;
        io.trafficOut --> sink.in;
}