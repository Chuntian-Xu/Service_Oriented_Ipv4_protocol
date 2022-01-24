// src/inet/networklayer/ipv4/Ipv4_ShoreStation.ned 

package inet.networklayer.ipv4;

moduleinterface IIpv4_ShoreStation
{
    gates:
        input transportIn @labels(Ipv4ControlInfo/down,TcpHeader,UdpHeader,SctpHeader);
        output transportOut @labels(Ipv4ControlInfo/up,TcpHeader,UdpHeader,SctpHeader);
        input queueIn @labels(Ipv4Header,ArpPacket,Ieee802Ctrl);
        output queueOut @labels(Ipv4Header,ArpPacket,Ieee802Ctrl);
}

simple Ipv4_ShoreStation like IIpv4_ShoreStation
{
    parameters:
        string interfaceTableModule;   // The path to the InterfaceTable module
        string routingTableModule;
        
        string sidTableModule;  // new added
        string cidTableModule;  // new added
        
        string arpModule;
        string icmpModule;
        string crcMode @enum("declared", "computed") = default("declared");
        double procDelay @unit(s) = default(0s);
        int timeToLive = default(32);
        int multicastTimeToLive = default(32);
        double fragmentTimeout @unit(s) = default(60s);
        bool limitedBroadcast = default(false); // send out limited broadcast packets comming from higher layer
        string directBroadcastInterfaces = default("");   // list of interfaces that direct broadcast is enabled (by default direct broadcast is disabled on all interfaces)
        @display("i=block/routing");
        @signal[packetSentToUpper](type=cPacket);
        @signal[packetReceivedFromUpper](type=cPacket);
        @signal[packetSentToLower](type=cPacket);
        @signal[packetReceivedFromLower](type=cPacket);
        @signal[packetDropped](type=cPacket);
        @signal[ipv4NewMulticast](type=inet::Ipv4Header);
        @signal[ipv4DataOnNonrpf](type=inet::Ipv4Header);
        @signal[ipv4DataOnRpf](type=inet::Ipv4Header);
        @signal[ipv4MdataRegister](type=inet::Packet);
        @statistic[packetDropAddressResolutionFailed](title="packet drop: address resolution failed"; source=packetDropReasonIsAddressResolutionFailed(packetDropped); record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);
        @statistic[packetDropHopLimitReached](title="packet drop: hop limit reached"; source=packetDropReasonIsHopLimitReached(packetDropped); record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);
        @statistic[packetDropForwardingDisabled](title="packet drop: forwarding disabled"; source=packetDropReasonIsForwardingDisabled(packetDropped); record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);
        @statistic[packetDropNoInterfaceFound](title="packet drop: no interface found"; source=packetDropReasonIsNoInterfaceFound(packetDropped); record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);
        @statistic[packetDropNoRouteFound](title="packet drop: no route found"; source=packetDropReasonIsNoRouteFound(packetDropped); record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);
        @statistic[packetDropUndefined](title="packet drop: undefined"; source=packetDropReasonIsUndefined(packetDropped); record=count,sum(packetBytes),vector(packetBytes); interpolationmode=none);
    gates:
        input transportIn @labels(Ipv4ControlInfo/down,TcpHeader,UdpHeader,SctpHeader);
        output transportOut @labels(Ipv4ControlInfo/up,TcpHeader,UdpHeader,SctpHeader);
        input queueIn @labels(Ipv4Header,ArpPacket,Ieee802Ctrl);
        output queueOut @labels(Ipv4Header,ArpPacket,Ieee802Ctrl);
}
