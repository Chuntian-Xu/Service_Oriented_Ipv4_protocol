// src/inet/networklayer/ipv4/Ipv4NetworkLayer.ned

package inet.networklayer.ipv4;

import inet.common.MessageDispatcher;
import inet.networklayer.configurator.contract.IIpv4NodeConfigurator;
import inet.networklayer.contract.IArp;
import inet.networklayer.contract.INetworkLayer;

//
// Network layer of an IPv4 node.
//

module Ipv4NetworkLayer like INetworkLayer
{
    parameters:
        bool forwarding = default(false);
        bool multicastForwarding = default(false);
        string interfaceTableModule;
        string displayStringTextFormat = default("%i");
        *.forwarding = forwarding;
        *.multicastForwarding = multicastForwarding;
        *.interfaceTableModule = default(absPath(interfaceTableModule));
        *.routingTableModule = default(absPath(".routingTable"));

        *.sidTableModule = default(absPath(".sidTable")); // new added
        *.configureSidTable = false; // new added

        *.cidshoreTableModule = default(absPath(".cidshoreTable")); // new added
        *.configureCidshoreTable = false; // new added

        *.arpModule = default(absPath(".arp"));
        *.icmpModule = default(absPath(".icmp"));
        @class(::inet::Ipv4NetworkLayer);
        @display("i=block/fork");

    gates:
        input ifIn @labels(INetworkHeader);
        output ifOut @labels(INetworkHeader);
        input transportIn @labels(Ipv4ControlInfo/down);
        output transportOut @labels(Ipv4ControlInfo/up);

    submodules:
        //  the network's global configurator module
        configurator_Node: <default("Ipv4NodeConfigurator")> like IIpv4NodeConfigurator if typename != "" {
            parameters:
                @display("p=109.2,100.1;is=s");
        }
        routingTable: Ipv4RoutingTable {
            parameters:
                @display("p=109.2,198.9;is=s");
        }

        // ServiceId Table module  // new added
        sidTable: Ipv4SidTable {
            parameters:
                @display("p=109,392;is=s");
        }

        // ClientId_shore Table module  // new added
        cidshoreTable: Ipv4CidshoreTable {
            parameters:
                @display("p=109,464;is=s");
        }

        natTable: Ipv4NatTable {
            parameters:
                @display("p=109,301;is=s");
        }
        up: MessageDispatcher {
            parameters:
                @display("p=550,100;b=600,5");
        }
        igmp: <default("Igmpv2")> like IIgmp {
            parameters:
                @display("p=400,200");
        }
        icmp: Icmp {
            parameters:
                @display("p=700,200");
        }
        mp: MessageDispatcher {
            parameters:
                @display("p=550,300;b=600,5");
        }
        ip: <default("Ipv4")> like IIpv4 {
            parameters:
                @display("p=700,400;q=queue");
        }
        arp: <default("Arp")> like IArp {
            parameters:
                @display("p=400,400;q=pendingQueue");
        }
        lp: MessageDispatcher {
            parameters:
                @display("p=550,500;b=600,5");
        }

    connections allowunconnected:
        transportIn --> { @display("m=n"); } --> up.in++;
        transportOut <-- { @display("m=n"); } <-- up.out++;

        up.out++ --> igmp.routerIn;
        up.in++ <-- igmp.routerOut;

        up.out++ --> mp.in++;
        up.in++ <-- mp.out++;

        up.out++ --> icmp.transportIn;
        up.in++ <-- icmp.transportOut;

        igmp.ipOut --> mp.in++;
        igmp.ipIn <-- mp.out++;

        icmp.ipOut --> mp.in++;
        icmp.ipIn <-- mp.out++;

        mp.out++ --> ip.transportIn;
        mp.in++ <-- ip.transportOut;

        arp.ifOut --> lp.in++;
        arp.ifIn <-- lp.out++;

        ip.queueOut --> lp.in++;
        ip.queueIn <-- lp.out++;

        lp.out++ --> { @display("m=s"); } --> ifOut;
        lp.in++ <-- { @display("m=s"); } <-- ifIn;
}

