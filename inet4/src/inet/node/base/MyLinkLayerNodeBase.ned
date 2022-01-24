// src/inet/node/base/MyLinkLayerNodeBase.ned
 
package inet.node.base;

import inet.common.packet.recorder.PcapRecorder;
import inet.linklayer.contract.IEthernetInterface;
import inet.linklayer.contract.IExternalInterface;
import inet.linklayer.contract.ILoopbackInterface;
import inet.linklayer.contract.IPppInterface;
import inet.linklayer.contract.ITunnelInterface;
import inet.linklayer.contract.IWirelessInterface;
import inet.linklayer.contract.IVlanInterface;
import inet.networklayer.common.InterfaceTable;

module MyLinkLayerNodeBase extends NodeBase
{
    parameters:
        int numPcapRecorders = default(0);
        int numLoInterfaces = default(1);
        int numWmtcInterfaces = default(0);
        int numEthInterfaces = default(0);  // minimum number of ethernet interfaces
        int numPppInterfaces = default(0);  // minimum number of PPP interfaces
        int numTunInterfaces = default(0);
        int numVlanInterfaces = default(0);
        mobility.typename = default(numWmtcInterfaces > 0 ? "StationaryMobility" : "");
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        wmtc[*].radio.antenna.mobilityModule = default("^.^.^.mobility");
        @figure[linkLayer](type=rectangle; pos=250,456; size=1000,210; fillColor=#0000ff; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[linkLayer.title](type=text; pos=1245,461; anchor=ne; text="link layer");

        @display("bgb=1713.0253,680.9453");
    gates:
        input radioIn[numWmtcInterfaces] @directIn;
        inout pppg[numPppInterfaces] @labels(PppFrame-conn) @allowUnconnected;
        inout ethg[numEthInterfaces] @labels(EtherFrame-conn) @allowUnconnected;

    submodules:
        pcapRecorder[numPcapRecorders]: PcapRecorder {
            parameters:
                @display("p=211.04999,548.1;is=s");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=211.04999,244.12498;is=s");
        }
        lo[numLoInterfaces]: <default("LoopbackInterface")> like ILoopbackInterface {
            parameters:
                @display("p=1013.5992,615.5518,row,150");
        }
        wmtc[numWmtcInterfaces]: <default("AckingWirelessInterface")> like IWirelessInterface {
            parameters:
                @display("p=848.69385,503.2456,row,150;q=queue");
        }
        ppp[sizeof(pppg)]: <default("PppInterface")> like IPppInterface {
            parameters:
                @display("p=1168.5533,503.2456,row,150;q=txQueue");
        }
        eth[sizeof(ethg)]: <default("EthernetInterface")> like IEthernetInterface {
            parameters:
                @display("p=525.99115,503.2456,row,150;q=txQueue");
        }
        tun[numTunInterfaces]: <default("TunInterface")> like ITunnelInterface {
            parameters:
                @display("p=659.62134,615.5518,row,150;q=txQueue");
        }
        vlan[numVlanInterfaces]: <default("VlanInterface")> like IVlanInterface {
            parameters:
                @display("p=1333.4586,615.5518,row,150;q=txQueue");
        }

    connections allowunconnected:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wmtc[i].radioIn;
        }

        for i=0..sizeof(ethg)-1 {
            ethg[i] <--> { @display("m=s"); } <--> eth[i].phys;
        }

        for i=0..sizeof(pppg)-1 {
            pppg[i] <--> { @display("m=s"); } <--> ppp[i].phys;
        }
}
