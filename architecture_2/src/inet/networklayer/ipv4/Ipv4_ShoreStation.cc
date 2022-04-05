// src/inet/networklayer/ipv4/Ipv4_ShoreStation.cc

#include <stdlib.h>
#include <string.h>

#include "inet/applications/common/SocketTag_m.h"
#include "inet/common/INETUtils.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/LayeredProtocolBase.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/checksum/TcpIpChecksum.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/networklayer/arp/ipv4/ArpPacket_m.h"
#include "inet/networklayer/common/DscpTag_m.h"
#include "inet/networklayer/common/EcnTag_m.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/HopLimitTag_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/networklayer/common/L3Tools.h"
#include "inet/networklayer/common/MulticastTag_m.h"
#include "inet/networklayer/common/NextHopAddressTag_m.h"
#include "inet/networklayer/common/TosTag_m.h"
#include "inet/networklayer/contract/IArp.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/contract/ipv4/Ipv4SocketCommand_m.h"
#include "inet/networklayer/ipv4/IIpv4RoutingTable.h"
#include "inet/networklayer/ipv4/IcmpHeader_m.h"
#include "inet/networklayer/ipv4/Ipv4.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/ipv4/Ipv4OptionsTag_m.h"

#include "inet/networklayer/ipv4/IIpv4SidTable.h"  // new added
#include "inet/networklayer/ipv4/Ipv4ServiceHeader_m.h"  // new added
#include "Ipv4_ShoreStation.h"  // new added

#include "inet/transportlayer/common/L4PortTag_m.h" // new added
#include "inet/transportlayer/common/L4Tools.h" // new added
#include "inet/transportlayer/udp/Udp.h" // new added
#include "inet/transportlayer/udp/UdpHeader_m.h"  // new added

#include "inet/networklayer/ipv4/IIpv4CidshoreTable.h"  // new added

namespace inet {

Define_Module(Ipv4_ShoreStation);

Ipv4_ShoreStation::Ipv4_ShoreStation()
{
}

Ipv4_ShoreStation::~Ipv4_ShoreStation()
{
    for (auto it : socketIdToSocketDescriptor) delete it.second;
    flush();
}

// ^-^
void Ipv4_ShoreStation::initialize(int stage) {
    OperationalBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        rt = getModuleFromPar<IIpv4RoutingTable>(par("routingTableModule"), this);

        st = getModuleFromPar<IIpv4SidTable>(par("sidTableModule"), this); // new added
        ctshore = getModuleFromPar<IIpv4CidshoreTable>(par("cidshoreTableModule"), this); // new added

        arp = getModuleFromPar<IArp>(par("arpModule"), this);
        icmp = getModuleFromPar<Icmp>(par("icmpModule"), this);

        transportInGateBaseId = gateBaseId("transportIn");

        const char *crcModeString = par("crcMode");
        crcMode = parseCrcMode(crcModeString, false);

        defaultTimeToLive = par("timeToLive");
        defaultMCTimeToLive = par("multicastTimeToLive");
        fragmentTimeoutTime = par("fragmentTimeout");
        limitedBroadcast = par("limitedBroadcast");
        directBroadcastInterfaces = par("directBroadcastInterfaces").stdstringValue();

        directBroadcastInterfaceMatcher.setPattern(directBroadcastInterfaces.c_str(), false, true, false);

        curFragmentId = 0;
        lastCheckTime = 0;

        numMulticast = numLocalDeliver = numDropped = numUnroutable = numForwarded = 0;

        // NetFilter:
        hooks.clear();
        queuedDatagramsForHooks.clear();

        pendingPackets.clear();
        cModule *arpModule = check_and_cast<cModule *>(arp);
        arpModule->subscribe(IArp::arpResolutionCompletedSignal, this);
        arpModule->subscribe(IArp::arpResolutionFailedSignal, this);

        registerService(Protocol::ipv4, gate("transportIn"), gate("queueIn"));
        registerProtocol(Protocol::ipv4, gate("queueOut"), gate("transportOut"));

        WATCH(numMulticast);
        WATCH(numLocalDeliver);
        WATCH(numDropped);
        WATCH(numUnroutable);
        WATCH(numForwarded);
        WATCH_MAP(pendingPackets);
        WATCH_MAP(socketIdToSocketDescriptor);
    }
}

void Ipv4_ShoreStation::handleRegisterService(const Protocol& protocol, cGate *out, ServicePrimitive servicePrimitive)
{
    Enter_Method("handleRegisterService");
}

void Ipv4_ShoreStation::handleRegisterProtocol(const Protocol& protocol, cGate *in, ServicePrimitive servicePrimitive)
{
    Enter_Method("handleRegisterProtocol");
    if (in->isName("transportIn"))
            upperProtocols.insert(&protocol);
}

void Ipv4_ShoreStation::refreshDisplay() const
{
    OperationalBase::refreshDisplay();

    char buf[80] = "";
    if (numForwarded > 0)
        sprintf(buf + strlen(buf), "fwd:%d ", numForwarded);
    if (numLocalDeliver > 0)
        sprintf(buf + strlen(buf), "up:%d ", numLocalDeliver);
    if (numMulticast > 0)
        sprintf(buf + strlen(buf), "mcast:%d ", numMulticast);
    if (numDropped > 0)
        sprintf(buf + strlen(buf), "DROP:%d ", numDropped);
    if (numUnroutable > 0)
        sprintf(buf + strlen(buf), "UNROUTABLE:%d ", numUnroutable);
    getDisplayString().setTagArg("t", 0, buf);
}

// ^-^
void Ipv4_ShoreStation::handleRequest(Request *request) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::handleRequest(Request *request) \n"; // new added
    auto ctrl = request->getControlInfo();
    if (ctrl == nullptr)
        throw cRuntimeError("Request '%s' arrived without controlinfo", request->getName());
    else if (Ipv4SocketBindCommand *command = dynamic_cast<Ipv4SocketBindCommand *>(ctrl)) {
        int socketId = request->getTag<SocketReq>()->getSocketId();
        SocketDescriptor *descriptor = new SocketDescriptor(socketId, command->getProtocol()->getId(), command->getLocalAddress());
        socketIdToSocketDescriptor[socketId] = descriptor;
        delete request;
    }
    else if (Ipv4SocketConnectCommand *command = dynamic_cast<Ipv4SocketConnectCommand *>(ctrl)) {
        int socketId = request->getTag<SocketReq>()->getSocketId();
        if (socketIdToSocketDescriptor.find(socketId) == socketIdToSocketDescriptor.end())
            throw cRuntimeError("Ipv4Socket: should use bind() before connect()");
        socketIdToSocketDescriptor[socketId]->remoteAddress = command->getRemoteAddress();
        delete request;
    }
    else if (dynamic_cast<Ipv4SocketCloseCommand *>(ctrl) != nullptr) {
        int socketId = request->getTag<SocketReq>()->getSocketId();
        auto it = socketIdToSocketDescriptor.find(socketId);
        if (it != socketIdToSocketDescriptor.end()) {
            delete it->second;
            socketIdToSocketDescriptor.erase(it);
            auto indication = new Indication("closed", IPv4_I_SOCKET_CLOSED);
            auto ctrl = new Ipv4SocketClosedIndication();
            indication->setControlInfo(ctrl);
            indication->addTag<SocketInd>()->setSocketId(socketId);
            send(indication, "transportOut");
        }
        delete request;
    }
    else if (dynamic_cast<Ipv4SocketDestroyCommand *>(ctrl) != nullptr) {
        int socketId = request->getTag<SocketReq>()->getSocketId();
        auto it = socketIdToSocketDescriptor.find(socketId);
        if (it != socketIdToSocketDescriptor.end()) {
            delete it->second;
            socketIdToSocketDescriptor.erase(it);
        }
        delete request;
    }
    else
        throw cRuntimeError("Unknown command: '%s' with %s", request->getName(), ctrl->getClassName());
}

// ^-^
void Ipv4_ShoreStation::handleMessageWhenUp(cMessage *msg) {
    EV_INFO<<"!!! --> Ipv4_ShoreStation::handleMessageWhenUp(cMessage *msg)\n"; // new added
    if (msg->arrivedOn("transportIn")) {    //TODO packet->getArrivalGate()->getBaseId() == transportInGateBaseId
        if (auto request = dynamic_cast<Request *>(msg)) handleRequest(request);
        else handlePacketFromHL(check_and_cast<Packet*>(msg));
    }
    else if (msg->arrivedOn("queueIn")) {    // from network
        EV_INFO << "Received " << msg << " from network.\n";
        // ^-^ new added !!! 
        Packet* packet = check_and_cast<Packet*>(msg);
        EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
        if(packet->hasAtFront<Ipv4Header>()) {
            handleIncomingDatagram(packet);
        }
        else if(packet->hasAtFront<Ipv4ServiceHeader>()) {
            handleIncomingDatagram_Service(packet);
        }

    }
    else throw cRuntimeError("message arrived on unknown gate '%s'", msg->getArrivalGate()->getName());
}

bool Ipv4_ShoreStation::verifyCrc(const Ptr<const Ipv4Header>& ipv4Header)
{
    switch (ipv4Header->getCrcMode()) {
        case CRC_DECLARED_CORRECT: {
            // if the CRC mode is declared to be correct, then the check passes if and only if the chunk is correct
            return ipv4Header->isCorrect();
        }
        case CRC_DECLARED_INCORRECT:
            // if the CRC mode is declared to be incorrect, then the check fails
            return false;
        case CRC_COMPUTED: {
            if (ipv4Header->isCorrect()) {
                // compute the CRC, the check passes if the result is 0xFFFF (includes the received CRC) and the chunks are correct
                MemoryOutputStream ipv4HeaderStream;
                Chunk::serialize(ipv4HeaderStream, ipv4Header);
                uint16_t computedCrc = TcpIpChecksum::checksum(ipv4HeaderStream.getData());
                return computedCrc == 0;
            }
            else {
                return false;
            }
        }
        default:
            throw cRuntimeError("Unknown CRC mode");
    }
}

const InterfaceEntry *Ipv4_ShoreStation::getSourceInterface(Packet *packet)
{
//    EV_INFO<<"!!! --> InterfaceEntry *Ipv4_ShoreStation::getSourceInterface(Packet *packet)\n"; // new added
    auto tag = packet->findTag<InterfaceInd>();
    return tag != nullptr ? ift->getInterfaceById(tag->getInterfaceId()) : nullptr;
}

// ^-^
const InterfaceEntry *Ipv4_ShoreStation::getDestInterface(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::getDestInterface(Packet *packet)\n"; // new added
    auto tag = packet->findTag<InterfaceReq>();
    return tag != nullptr ? ift->getInterfaceById(tag->getInterfaceId()) : nullptr;
}

// ^-^
Ipv4Address Ipv4_ShoreStation::getNextHop(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::getNextHop(Packet *packet)\n"; // new added
    auto tag = packet->findTag<NextHopAddressReq>();
    return tag != nullptr ? tag->getNextHopAddress().toIpv4() : Ipv4Address::UNSPECIFIED_ADDRESS;
}

// ^-^
void Ipv4_ShoreStation::handleIncomingDatagram(Packet *packet) {
    service_flag = true; // ^-^ new added !!!
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::handleIncomingDatagram(Packet *packet) --> service_flag == true\n"; // new added
    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    ASSERT(packet);
    int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();
    emit(packetReceivedFromLowerSignal, packet);
    // "Prerouting"
    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();  // !!!--> ��ȡ ipv4Header
    packet->addTagIfAbsent<NetworkProtocolInd>()->setProtocol(&Protocol::ipv4);
    packet->addTagIfAbsent<NetworkProtocolInd>()->setNetworkProtocolHeader(ipv4Header);
    if (!verifyCrc(ipv4Header)) {
        EV_WARN << "CRC error found, drop packet\n";
        PacketDropDetails details;
        details.setReason(INCORRECTLY_RECEIVED);
        emit(packetDroppedSignal, packet, &details);
        delete packet;
        return;
    }
    if (ipv4Header->getTotalLengthField() > packet->getDataLength()) {
        EV_WARN << "length error found, sending ICMP_PARAMETER_PROBLEM\n";
        sendIcmpError(packet, interfaceId, ICMP_PARAMETER_PROBLEM, 0);
        return;
    }
    // remove lower layer paddings:
    if (ipv4Header->getTotalLengthField() < packet->getDataLength()) {
        packet->setBackOffset(packet->getFrontOffset() + ipv4Header->getTotalLengthField());
    }
    // check for header biterror
    if (packet->hasBitError()) {
        // probability of bit error in header = size of header / size of total message
        // (ignore bit error if in payload)
        double relativeHeaderLength = B(ipv4Header->getHeaderLength()).get() / (double)B(ipv4Header->getChunkLength()).get();
        if (dblrand() <= relativeHeaderLength) {
            EV_WARN << "bit error found, sending ICMP_PARAMETER_PROBLEM\n";
            sendIcmpError(packet, interfaceId, ICMP_PARAMETER_PROBLEM, 0);
            return;
        }
    }
    EV_DETAIL << "Received datagram `" << ipv4Header->getName() << "' with dest=" << ipv4Header->getDestAddress() << "\n";

    // new added !!! Remove the physical layer and Mac layer headers
    const auto& ipv4Header_ = removeNetworkProtocolHeader<Ipv4Header>(packet);
    insertNetworkProtocolHeader(packet, Protocol::ipv4, ipv4Header_);
    sendDatagramToOutput_Service(packet);
}

// ^-^ new added !!! Remove and parse the Ipv4serviceHeader, reconstruct the UdpHeader and Ipv4Header, and then send it to the destination host through routing
void Ipv4_ShoreStation::handleIncomingDatagram_Service(Packet *packet) {
    service_flag = false; // new added
    EV_INFO<<"!!! --> Ipv4_ShoreStation::handleIncomingDatagram_Service(Packet *packet) --> service_flag == false\n"; // new added
    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    ASSERT(packet);
    auto macAddressInd = packet->getTag<MacAddressInd>();
    EV_INFO<<"    --> src mac: "<<macAddressInd->getSrcAddress()<<endl;
    EV_INFO<<"    --> dest mac: "<<macAddressInd->getDestAddress()<<endl;
    emit(packetReceivedFromLowerSignal, packet);

    //----------------- Remove and parse Ipv4ServiceHeader -----------------
    const auto& ipv4ServiceHeader = removeNetworkProtocolHeader<Ipv4ServiceHeader>(packet);
    EV_INFO<<"    --> packet after removeIpv4ServiceHeader: "<<packet<<"\n"; // new added
    ServiceId serviceId=ipv4ServiceHeader->getServiceId();
    EV_INFO << "    --> ipv4ServiceHeader (" << ipv4ServiceHeader->getChunkLength() << ")" << " [serviceId: " << serviceId << "]" << endl;

    //----------------- Inquiry the sid_table and cidshore_table of ShoreStation for srcIpaddr, srcPort and destIpaddr, desrPort -----------------
    int cidshoreNum = ctshore->getNumCidshores();
    int sidNum = st->getNumSids();
    Ipv4Address srcIpaddr;
    Ipv4Address destIpaddr;
    u_short srcPort;
    u_short destPort;
    for(int i=0; i<cidshoreNum; ++i) {
        Ipv4Cidshore* cidsh = ctshore->getCidshore(i);
        if(cidsh->getMMSI() == macAddressInd->getSrcAddress()) {
            srcIpaddr = cidsh->getIpaddr();
            srcPort = cidsh->getPort();
            EV_INFO<<"    --> MMSI: "<< cidsh->getMMSI() << "  srcIpaddr: "<< srcIpaddr << "  srcPort: "<< srcPort << endl;
        }
    }
    for(int i=0; i<sidNum; ++i) {
        Ipv4Sid* sid = st->getSid(i);
        if(sid->getSid() == serviceId) {
            destIpaddr = sid->getIpaddr();
            destPort = sid->getPort();
            EV_INFO<<"    --> serviceId: "<< sid->getSid() <<"  destIpaddr: "<< destIpaddr <<"  destPort: "<< destPort << endl;
        }
    }

//    //---------------- Add tag, send PayLoad to Udp layer to encapsulate UdpHeader ----------------
//    EV_INFO << "Passing up to Udp Layer " << endl;
//    auto l3AddressReq = packet->addTagIfAbsent<L3AddressReq>();
//    l3AddressReq->setSrcAddress(srcIpaddr);
//    l3AddressReq->setDestAddress(destIpaddr);
//    auto portsReq = packet->addTagIfAbsent<L4PortReq>();
//    portsReq->setSrcPort(srcPort);
//    portsReq->setDestPort(destPort);
//    auto dispatchProtocolReq = packet->addTagIfAbsent<DispatchProtocolReq>();
//    dispatchProtocolReq->setProtocol(&Protocol::ipv4_service);
//    auto packetProtocolTag = packet->addTagIfAbsent<PacketProtocolTag>();
//    packetProtocolTag->setProtocol(&Protocol::ipv4_service);
//    send(packet, "transportOut");

    //---------------- reconstruct and add UdpHeader ----------------
    auto udpHeader = makeShared<UdpHeader>();
    udpHeader->setSourcePort(srcPort);
    udpHeader->setDestinationPort(destPort);
    B totalLength = udpHeader->getChunkLength() + packet->getTotalLength();
    if(totalLength.get() > UDP_MAX_MESSAGE_SIZE)
        throw cRuntimeError("send: total UDP message size exceeds %u", UDP_MAX_MESSAGE_SIZE);
    udpHeader->setTotalLengthField(totalLength);
    if (crcMode == CRC_COMPUTED) {
        udpHeader->setCrcMode(CRC_COMPUTED);
        udpHeader->setCrc(0x0000);    // crcMode == CRC_COMPUTED is done in an INetfilter hook
    }
    else {
        udpHeader->setCrcMode(crcMode);
    }
    insertTransportProtocolHeader(packet, Protocol::udp, udpHeader);
    packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);
    EV_INFO<<"    --> packet after insert UdpHeader: "<< packet <<endl; // new added

    //---------------- reconstruct and add Ipv4Header ----------------
    const auto& ipv4Header = makeShared<Ipv4Header>();
    ipv4Header->setSrcAddress(srcIpaddr);
    ipv4Header->setDestAddress(destIpaddr);
    ipv4Header->setProtocolId((IpProtocolId)17); //
    auto hopLimitReq = packet->removeTagIfPresent<HopLimitReq>();
    short ttl = (hopLimitReq != nullptr) ? hopLimitReq->getHopLimit() : -1;
    delete hopLimitReq;
    bool dontFragment = false;
    if (auto dontFragmentReq = packet->removeTagIfPresent<FragmentationReq>()) {
        dontFragment = dontFragmentReq->getDontFragment();
        delete dontFragmentReq;
    }
    // set other fields
    if (TosReq *tosReq = packet->removeTagIfPresent<TosReq>()) {
        ipv4Header->setTypeOfService(tosReq->getTos());
        delete tosReq;
        if (packet->findTag<DscpReq>()) throw cRuntimeError("TosReq and DscpReq found together");
        if (packet->findTag<EcnReq>()) throw cRuntimeError("TosReq and EcnReq found together");
    }
    if (DscpReq *dscpReq = packet->removeTagIfPresent<DscpReq>()) {
        ipv4Header->setDscp(dscpReq->getDifferentiatedServicesCodePoint());
        delete dscpReq;
    }
    if (EcnReq *ecnReq = packet->removeTagIfPresent<EcnReq>()) {
        ipv4Header->setEcn(ecnReq->getExplicitCongestionNotification());
        delete ecnReq;
    }
    ipv4Header->setIdentification(curFragmentId++);
    ipv4Header->setMoreFragments(false);
    ipv4Header->setDontFragment(dontFragment);
    ipv4Header->setFragmentOffset(0);
    if (ttl != -1) ASSERT(ttl > 0);
    else if (ipv4Header->getDestAddress().isLinkLocalMulticast()) ttl = 1;
    else if (ipv4Header->getDestAddress().isMulticast()) ttl = defaultMCTimeToLive;
    else ttl = defaultTimeToLive;
    ipv4Header->setTimeToLive(ttl);
    if (Ipv4OptionsReq *optReq = packet->removeTagIfPresent<Ipv4OptionsReq>()) {
        for (size_t i = 0; i < optReq->getOptionArraySize(); i++) {
            auto opt = optReq->dropOption(i);
            ipv4Header->addOption(opt);
            ipv4Header->addChunkLength(B(opt->getLength()));
        }
        delete optReq;
    }
    ASSERT(ipv4Header->getChunkLength() <= IPv4_MAX_HEADER_LENGTH);
    ipv4Header->setHeaderLength(ipv4Header->getChunkLength());
    ipv4Header->setTotalLengthField(ipv4Header->getChunkLength() + packet->getDataLength());
    ipv4Header->setCrcMode(crcMode);
    ipv4Header->setCrc(0);
    switch (crcMode) {
        case CRC_DECLARED_CORRECT:
            ipv4Header->setCrc(0xC00D);
            break;
        case CRC_DECLARED_INCORRECT:
            ipv4Header->setCrc(0xBAAD);
            break;
        case CRC_COMPUTED: {
            ipv4Header->setCrc(0);
            break;
        }
        default:
            throw cRuntimeError("Unknown CRC mode");
    }
    insertNetworkProtocolHeader(packet, Protocol::ipv4, ipv4Header);
    EV_INFO<<"    --> packet after insert Ipv4Header: "<< packet <<endl; // new added

    //---------------- Routing IP packets ----------------
    EV_DETAIL << "Received datagram `" << ipv4Header->getName() << "' with dest=" << ipv4Header->getDestAddress() << "\n";
    if (datagramPreRoutingHook(packet) == INetfilter::IHook::ACCEPT) preroutingFinish(packet);
}

// ^-^
Packet *Ipv4_ShoreStation::prepareForForwarding(Packet *packet) const {
    EV_INFO<<"   !--> Ipv4_ShoreStation::prepareForForwarding(Packet *packet) \n"; // new added
//    EV_INFO<<"    --> packet before removeNetworkProtocolHeader(): "<<packet<<"\n"; // new added
    const auto& ipv4Header = removeNetworkProtocolHeader<Ipv4Header>(packet); // !!!!!!
//    EV_INFO<<"    --> packet afer removeNetworkProtocolHeader(): "<<packet<<"\n"; // new added
    ipv4Header->setTimeToLive(ipv4Header->getTimeToLive() - 1);
    insertNetworkProtocolHeader(packet, Protocol::ipv4, ipv4Header);  // !!!!!!
//    EV_INFO<<"    --> packet afer insertNetworkProtocolHeader(): "<<packet<<"\n"; // new added
    return packet;
}

// ^-^
void Ipv4_ShoreStation::preroutingFinish(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::preroutingFinish(Packet *packet)\n"; // new added
//    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    const InterfaceEntry *fromIE = ift->getInterfaceById(packet->getTag<InterfaceInd>()->getInterfaceId());
    Ipv4Address nextHopAddr = getNextHop(packet);

    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();

    ASSERT(ipv4Header);
    Ipv4Address destAddr = ipv4Header->getDestAddress();
    // route packet
    if (fromIE->isLoopback()) {
        reassembleAndDeliver(packet);
    }
    else if (destAddr.isMulticast()) {
        // check for local delivery
        // Note: multicast routers will receive IGMP datagrams even if their interface is not joined to the group
        if (fromIE->getProtocolData<Ipv4InterfaceData>()->isMemberOfMulticastGroup(destAddr) ||
            (rt->isMulticastForwardingEnabled() && ipv4Header->getProtocolId() == IP_PROT_IGMP))
            reassembleAndDeliver(packet->dup());
        else
            EV_WARN << "Skip local delivery of multicast datagram (input interface not in multicast group)\n";

        // don't forward if IP forwarding is off, or if dest address is link-scope
        if (!rt->isMulticastForwardingEnabled()) {
            EV_WARN << "Skip forwarding of multicast datagram (forwarding disabled)\n";
            delete packet;
        }
        else if (destAddr.isLinkLocalMulticast()) {
            EV_WARN << "Skip forwarding of multicast datagram (packet is link-local)\n";
            delete packet;
        }
        else if (ipv4Header->getTimeToLive() <= 1) {      // TTL before decrement
            EV_WARN << "Skip forwarding of multicast datagram (TTL reached 0)\n";
            delete packet;
        }
        else
            forwardMulticastPacket(prepareForForwarding(packet));
    }
    else {
        const InterfaceEntry *broadcastIE = nullptr;
        // check for local delivery; we must accept also packets coming from the interfaces that
        // do not yet have an IP address assigned. This happens during DHCP requests.
        if (rt->isLocalAddress(destAddr) || fromIE->getProtocolData<Ipv4InterfaceData>()->getIPAddress().isUnspecified()) {
//            EV_INFO<<"    --> isLocalAddress\n";// new added
            reassembleAndDeliver(packet);
        }
        else if (destAddr.isLimitedBroadcastAddress() || (broadcastIE = rt->findInterfaceByLocalBroadcastAddress(destAddr))) {
            // broadcast datagram on the target subnet if we are a router
            if (broadcastIE && fromIE != broadcastIE && rt->isForwardingEnabled()) {
                if (directBroadcastInterfaceMatcher.matches(broadcastIE->getInterfaceName()) ||
                    directBroadcastInterfaceMatcher.matches(broadcastIE->getInterfaceFullPath().c_str()))
                {
                    auto packetCopy = prepareForForwarding(packet->dup());
                    packetCopy->addTagIfAbsent<InterfaceReq>()->setInterfaceId(broadcastIE->getInterfaceId());
                    packetCopy->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(Ipv4Address::ALLONES_ADDRESS);
                    fragmentPostRouting(packetCopy);
                }
                else
                    EV_INFO << "Forwarding of direct broadcast packets is disabled on interface " << broadcastIE->getInterfaceName() << std::endl;
            }
            EV_INFO << "Broadcast received\n";
            reassembleAndDeliver(packet);
        }
        else if (!rt->isForwardingEnabled()) {
            EV_WARN << "forwarding off, dropping packet\n";
            numDropped++;
            PacketDropDetails details;
            details.setReason(FORWARDING_DISABLED);
            emit(packetDroppedSignal, packet, &details);
            delete packet;
        }
        else {
            packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(nextHopAddr);
//            EV_INFO << "    --> prepareForForwarding!\n"; // new added
            routeUnicastPacket(prepareForForwarding(packet));
        }
    }
}

// ^-^
void Ipv4_ShoreStation::handlePacketFromHL(Packet *packet) {
//    EV_INFO <<"!!! --> Ipv4_ShoreStation::handlePacketFromHL(Packet *packet)" << "\n"; // new added
    EV_INFO << "Received " << packet << " from upper layer.\n";
    emit(packetReceivedFromUpperSignal, packet);

    // if no interface exists, do not send datagram
    if (ift->getNumInterfaces() == 0) {
        EV_ERROR << "No interfaces exist, dropping packet\n";
        numDropped++;
        PacketDropDetails details;
        details.setReason(NO_INTERFACE_FOUND);
        emit(packetDroppedSignal, packet, &details);
        delete packet;
        return;
    }
    // encapsulate
    encapsulate(packet);
    // TODO:
    L3Address nextHopAddr(Ipv4Address::UNSPECIFIED_ADDRESS);
    if (datagramLocalOutHook(packet) == INetfilter::IHook::ACCEPT) datagramLocalOut(packet);
}

// ^-^
void Ipv4_ShoreStation::datagramLocalOut(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::datagramLocalOut(Packet *packet)\n"; // new added
    const InterfaceEntry *destIE = getDestInterface(packet);
    Ipv4Address requestedNextHopAddress = getNextHop(packet);

    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
    bool multicastLoop = false;
    MulticastReq *mcr = packet->findTag<MulticastReq>();
    if (mcr != nullptr) {
        multicastLoop = mcr->getMulticastLoop();
    }

    // send
    Ipv4Address destAddr = ipv4Header->getDestAddress();

    EV_DETAIL << "Sending datagram '" << packet->getName() << "' with destination = " << destAddr << "\n";

    if (ipv4Header->getDestAddress().isMulticast()) {
        destIE = determineOutgoingInterfaceForMulticastDatagram(ipv4Header, destIE);
        packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(destIE ? destIE->getInterfaceId() : -1);
        // loop back a copy
        if (multicastLoop && (!destIE || !destIE->isLoopback())) {
            const InterfaceEntry *loopbackIF = ift->findFirstLoopbackInterface();
            if (loopbackIF) {
                auto packetCopy = packet->dup();
                packetCopy->addTagIfAbsent<InterfaceReq>()->setInterfaceId(loopbackIF->getInterfaceId());
                packetCopy->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(destAddr);
                fragmentPostRouting(packetCopy);
            }
        }
        if (destIE) {
            numMulticast++;
            packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(destIE->getInterfaceId());        //FIXME KLUDGE is it needed?
            packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(destAddr);
            fragmentPostRouting(packet);
        }
        else {
            EV_ERROR << "No multicast interface, packet dropped\n";
            numUnroutable++;
            PacketDropDetails details;
            details.setReason(NO_INTERFACE_FOUND);
            emit(packetDroppedSignal, packet, &details);
            delete packet;
        }
    }
    else {    // unicast and broadcast
              // check for local delivery
        if (rt->isLocalAddress(destAddr)) {
            EV_INFO << "Delivering " << packet << " locally.\n";
            if (destIE && !destIE->isLoopback()) {
                EV_DETAIL << "datagram destination address is local, ignoring destination interface specified in the control info\n";
                destIE = nullptr;
                packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(-1);
            }
            if (!destIE) {
                destIE = ift->findFirstLoopbackInterface();
                packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(destIE ? destIE->getInterfaceId() : -1);
            }
            ASSERT(destIE);
            packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(destAddr);
            routeUnicastPacket(packet);
        }
        else if (destAddr.isLimitedBroadcastAddress() || rt->isLocalBroadcastAddress(destAddr))
            routeLocalBroadcastPacket(packet);
        else {
            packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(requestedNextHopAddress);
            routeUnicastPacket(packet);
        }
    }
}

/* Choose the outgoing interface for the muticast datagram:
 *   1. use the interface specified by MULTICAST_IF socket option (received in the control info)
 *   2. lookup the destination address in the routing table
 *   3. if no route, choose the interface according to the source address
 *   4. or if the source address is unspecified, choose the first MULTICAST interface
 */
const InterfaceEntry *Ipv4_ShoreStation::determineOutgoingInterfaceForMulticastDatagram(const Ptr<const Ipv4Header>& ipv4Header, const InterfaceEntry *multicastIFOption)
{
    const InterfaceEntry *ie = nullptr;
    if (multicastIFOption) {
        ie = multicastIFOption;
        EV_DETAIL << "multicast packet routed by socket option via output interface " << ie->getInterfaceName() << "\n";
    }
    if (!ie) {
        Ipv4Route *route = rt->findBestMatchingRoute(ipv4Header->getDestAddress());
        if (route)
            ie = route->getInterface();
        if (ie)
            EV_DETAIL << "multicast packet routed by routing table via output interface " << ie->getInterfaceName() << "\n";
    }
    if (!ie) {
        ie = rt->getInterfaceByAddress(ipv4Header->getSrcAddress());
        if (ie)
            EV_DETAIL << "multicast packet routed by source address via output interface " << ie->getInterfaceName() << "\n";
    }
    if (!ie) {
        ie = ift->findFirstMulticastInterface();
        if (ie)
            EV_DETAIL << "multicast packet routed via the first multicast interface " << ie->getInterfaceName() << "\n";
    }
    return ie;
}

// ^-^
void Ipv4_ShoreStation::routeUnicastPacket(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::routeUnicastPacket(Packet *packet)\n"; // new added
//    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    const InterfaceEntry *fromIE = getSourceInterface(packet);
    const InterfaceEntry *destIE = getDestInterface(packet);
    Ipv4Address nextHopAddress = getNextHop(packet);

    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
    Ipv4Address destAddr = ipv4Header->getDestAddress();
    EV_INFO << "Routing " << packet << " with destination = " << destAddr << ", ";

    // if output port was explicitly requested, use that, otherwise use Ipv4 routing
    if (destIE) {
        EV_DETAIL << "using manually specified output interface " << destIE->getInterfaceName() << "\n";
        // and nextHopAddr remains unspecified
        if (!nextHopAddress.isUnspecified()) {
            // do nothing, next hop address already specified
        }
        // special case ICMP reply
        else if (destIE->isBroadcast()) {
            // if the interface is broadcast we must search the next hop
            const Ipv4Route *re = rt->findBestMatchingRoute(destAddr);
            if (re && re->getInterface() == destIE) {
                packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(re->getGateway());
            }
        }
    }
    else {
        // use Ipv4 routing (lookup in routing table)
        const Ipv4Route *re = rt->findBestMatchingRoute(destAddr);
        if (re) {
            destIE = re->getInterface();
            packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(destIE->getInterfaceId());
            packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(re->getGateway());
        }
    }
    if (!destIE) {    // no route found
        EV_WARN << "unroutable, sending ICMP_DESTINATION_UNREACHABLE, dropping packet\n";
        numUnroutable++;
        PacketDropDetails details;
        details.setReason(NO_ROUTE_FOUND);
        emit(packetDroppedSignal, packet, &details);
        sendIcmpError(packet, fromIE ? fromIE->getInterfaceId() : -1, ICMP_DESTINATION_UNREACHABLE, 0);
    }
    else {    // fragment and send
        if (fromIE != nullptr) {
            if (datagramForwardHook(packet) != INetfilter::IHook::ACCEPT) return;
        }
//        EV_INFO << "\n    --> fragment and send\n"; // new added
        routeUnicastPacketFinish(packet);
    }
}
// ^-^
void Ipv4_ShoreStation::routeUnicastPacketFinish(Packet *packet) {
//    EV_INFO <<"!!! --> Ipv4_ShoreStation::routeUnicastPacketFinish(Packet *packet) \n"; // new added
//    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    EV_INFO << "output interface = " << getDestInterface(packet)->getInterfaceName() << ", next hop address = " << getNextHop(packet) << "\n";
    numForwarded++;
    fragmentPostRouting(packet);
}

void Ipv4_ShoreStation::routeLocalBroadcastPacket(Packet *packet)
{
    auto interfaceReq = packet->findTag<InterfaceReq>();
    const InterfaceEntry *destIE = interfaceReq != nullptr ? ift->getInterfaceById(interfaceReq->getInterfaceId()) : nullptr;
    // The destination address is 255.255.255.255 or local subnet broadcast address.
    // We always use 255.255.255.255 as nextHopAddress, because it is recognized by ARP,
    // and mapped to the broadcast MAC address.
    if (destIE != nullptr) {
        packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(destIE->getInterfaceId());    //FIXME KLUDGE is it needed?
        packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(Ipv4Address::ALLONES_ADDRESS);
        fragmentPostRouting(packet);
    }
    else if (limitedBroadcast) {
        auto destAddr = packet->peekAtFront<Ipv4Header>()->getDestAddress();
        // forward to each matching interfaces including loopback
        for (int i = 0; i < ift->getNumInterfaces(); i++) {
            const InterfaceEntry *ie = ift->getInterface(i);
            if (!destAddr.isLimitedBroadcastAddress()) {
                Ipv4Address interfaceAddr = ie->getProtocolData<Ipv4InterfaceData>()->getIPAddress();
                Ipv4Address broadcastAddr = interfaceAddr.makeBroadcastAddress(ie->getProtocolData<Ipv4InterfaceData>()->getNetmask());
                if (destAddr != broadcastAddr)
                    continue;
            }
            auto packetCopy = packet->dup();
            packetCopy->addTagIfAbsent<InterfaceReq>()->setInterfaceId(ie->getInterfaceId());
            packetCopy->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(Ipv4Address::ALLONES_ADDRESS);
            fragmentPostRouting(packetCopy);
        }
        delete packet;
    }
    else {
        numDropped++;
        PacketDropDetails details;
        details.setReason(NO_INTERFACE_FOUND);
        emit(packetDroppedSignal, packet, &details);
        delete packet;
    }
}

const InterfaceEntry *Ipv4_ShoreStation::getShortestPathInterfaceToSource(const Ptr<const Ipv4Header>& ipv4Header) const
{
    return rt->getInterfaceForDestAddr(ipv4Header->getSrcAddress());
}

void Ipv4_ShoreStation::forwardMulticastPacket(Packet *packet)
{
    const InterfaceEntry *fromIE = ift->getInterfaceById(packet->getTag<InterfaceInd>()->getInterfaceId());
    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
    const Ipv4Address& srcAddr = ipv4Header->getSrcAddress();
    const Ipv4Address& destAddr = ipv4Header->getDestAddress();
    ASSERT(destAddr.isMulticast());
    ASSERT(!destAddr.isLinkLocalMulticast());

    EV_INFO << "Forwarding multicast datagram `" << packet->getName() << "' with dest=" << destAddr << "\n";

    numMulticast++;

    const Ipv4MulticastRoute *route = rt->findBestMatchingMulticastRoute(srcAddr, destAddr);
    if (!route) {
        EV_WARN << "Multicast route does not exist, try to add.\n";
        // TODO: no need to emit fromIE when tags will be used in place of control infos
        emit(ipv4NewMulticastSignal, ipv4Header.get(), const_cast<InterfaceEntry *>(fromIE));

        // read new record
        route = rt->findBestMatchingMulticastRoute(srcAddr, destAddr);

        if (!route) {
            EV_ERROR << "No route, packet dropped.\n";
            numUnroutable++;
            PacketDropDetails details;
            details.setReason(NO_ROUTE_FOUND);
            emit(packetDroppedSignal, packet, &details);
            delete packet;
            return;
        }
    }

    if (route->getInInterface() && fromIE != route->getInInterface()->getInterface()) {
        EV_ERROR << "Did not arrive on input interface, packet dropped.\n";
        // TODO: no need to emit fromIE when tags will be used in place of control infos
        emit(ipv4DataOnNonrpfSignal, ipv4Header.get(), const_cast<InterfaceEntry *>(fromIE));
        numDropped++;
        PacketDropDetails details;
        emit(packetDroppedSignal, packet, &details);
        delete packet;
    }
    // backward compatible: no parent means shortest path interface to source (RPB routing)
    else if (!route->getInInterface() && fromIE != getShortestPathInterfaceToSource(ipv4Header)) {
        EV_ERROR << "Did not arrive on shortest path, packet dropped.\n";
        numDropped++;
        PacketDropDetails details;
        emit(packetDroppedSignal, packet, &details);
        delete packet;
    }
    else {
        // TODO: no need to emit fromIE when tags will be used in place of control infos
        emit(ipv4DataOnRpfSignal, ipv4Header.get(), const_cast<InterfaceEntry *>(fromIE));    // forwarding hook

        numForwarded++;
        // copy original datagram for multiple destinations
        for (unsigned int i = 0; i < route->getNumOutInterfaces(); i++) {
            Ipv4MulticastRoute::OutInterface *outInterface = route->getOutInterface(i);
            const InterfaceEntry *destIE = outInterface->getInterface();
            if (destIE != fromIE && outInterface->isEnabled()) {
                int ttlThreshold = destIE->getProtocolData<Ipv4InterfaceData>()->getMulticastTtlThreshold();
                if (ipv4Header->getTimeToLive() <= ttlThreshold)
                    EV_WARN << "Not forwarding to " << destIE->getInterfaceName() << " (ttl threshold reached)\n";
                else if (outInterface->isLeaf() && !destIE->getProtocolData<Ipv4InterfaceData>()->hasMulticastListener(destAddr))
                    EV_WARN << "Not forwarding to " << destIE->getInterfaceName() << " (no listeners)\n";
                else {
                    EV_DETAIL << "Forwarding to " << destIE->getInterfaceName() << "\n";
                    auto packetCopy = packet->dup();
                    packetCopy->addTagIfAbsent<InterfaceReq>()->setInterfaceId(destIE->getInterfaceId());
                    packetCopy->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(destAddr);
                    fragmentPostRouting(packetCopy);
                }
            }
        }

        // TODO: no need to emit fromIE when tags will be use, d in place of control infos
        emit(ipv4MdataRegisterSignal, packet, const_cast<InterfaceEntry *>(fromIE));    // postRouting hook

        // only copies sent, delete original packet
        delete packet;
    }
}

// ^-^
void Ipv4_ShoreStation::reassembleAndDeliver(Packet *packet) {
//    EV_INFO << "!!! --> Ipv4_ShoreStation::reassembleAndDeliver(Packet *packet) \n"; // new added
    EV_INFO << "Delivering " << packet << " locally.\n";

    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
    if (ipv4Header->getSrcAddress().isUnspecified())
        EV_WARN << "Received datagram '" << packet->getName() << "' without source address filled in\n";

    // reassemble the packet (if fragmented)
    if (ipv4Header->getFragmentOffset() != 0 || ipv4Header->getMoreFragments()) {
        EV_DETAIL << "Datagram fragment: offset=" << ipv4Header->getFragmentOffset()
                  << ", MORE=" << (ipv4Header->getMoreFragments() ? "true" : "false") << ".\n";

        // erase timed out fragments in fragmentation buffer; check every 10 seconds max
        if (simTime() >= lastCheckTime + 10) {
            lastCheckTime = simTime();
            fragbuf.purgeStaleFragments(icmp, simTime() - fragmentTimeoutTime);
        }

        packet = fragbuf.addFragment(packet, simTime());
        if (!packet) {
            EV_DETAIL << "No complete datagram yet.\n";
            return;
        }
        if (packet->peekAtFront<Ipv4Header>()->getCrcMode() == CRC_COMPUTED) {
            auto ipv4Header = removeNetworkProtocolHeader<Ipv4Header>(packet);
            setComputedCrc(ipv4Header);
            insertNetworkProtocolHeader(packet, Protocol::ipv4, ipv4Header);
        }
        EV_DETAIL << "This fragment completes the datagram.\n";
    }

    if (datagramLocalInHook(packet) == INetfilter::IHook::ACCEPT)
        reassembleAndDeliverFinish(packet);
}

// ^-^
void Ipv4_ShoreStation::reassembleAndDeliverFinish(Packet *packet) {
//    EV_INFO << "!!! --> Ipv4_ShoreStation::reassembleAndDeliverFinish(Packet *packet) \n"; // new added
    auto ipv4HeaderPosition = packet->getFrontOffset();
    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
    const Protocol *protocol = ipv4Header->getProtocol();
    auto remoteAddress(ipv4Header->getSrcAddress());
    auto localAddress(ipv4Header->getDestAddress());
    decapsulate(packet);
    bool hasSocket = false;
    for (const auto &elem: socketIdToSocketDescriptor) {
        if (elem.second->protocolId == protocol->getId()
                && (elem.second->localAddress.isUnspecified() || elem.second->localAddress == localAddress)
                && (elem.second->remoteAddress.isUnspecified() || elem.second->remoteAddress == remoteAddress)) {
            auto *packetCopy = packet->dup();
            packetCopy->setKind(IPv4_I_DATA);
            packetCopy->addTagIfAbsent<SocketInd>()->setSocketId(elem.second->socketId);
            EV_INFO << "Passing up to socket " << elem.second->socketId << "\n";
            emit(packetSentToUpperSignal, packetCopy);
            send(packetCopy, "transportOut");
            hasSocket = true;
        }
    }
    if (upperProtocols.find(protocol) != upperProtocols.end()) {
        EV_INFO << "Passing up to protocol " << *protocol << "\n";
        emit(packetSentToUpperSignal, packet);
        send(packet, "transportOut");
        numLocalDeliver++;
    }
    else if (hasSocket) {
        delete packet;
    }
    else {
        EV_ERROR << "Transport protocol '" << protocol->getName() << "' not connected, discarding packet\n";
        packet->setFrontOffset(ipv4HeaderPosition);
        const InterfaceEntry* fromIE = getSourceInterface(packet);
        sendIcmpError(packet, fromIE ? fromIE->getInterfaceId() : -1, ICMP_DESTINATION_UNREACHABLE, ICMP_DU_PROTOCOL_UNREACHABLE);
    }
}

// ^-^ !!! decapsulate ipv4Header
void Ipv4_ShoreStation::decapsulate(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::decapsulate(Packet *packet) \n"; // new added
    // decapsulate transport packet
    const auto& ipv4Header = packet->popAtFront<Ipv4Header>();

    // create and fill in control info
    packet->addTagIfAbsent<DscpInd>()->setDifferentiatedServicesCodePoint(ipv4Header->getDscp());
    packet->addTagIfAbsent<EcnInd>()->setExplicitCongestionNotification(ipv4Header->getEcn());
    packet->addTagIfAbsent<TosInd>()->setTos(ipv4Header->getTypeOfService());

    // original Ipv4 datagram might be needed in upper layers to send back ICMP error message

    auto transportProtocol = ProtocolGroup::ipprotocol.getProtocol(ipv4Header->getProtocolId());
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(transportProtocol);
    packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(transportProtocol);
    auto l3AddressInd = packet->addTagIfAbsent<L3AddressInd>();
    l3AddressInd->setSrcAddress(ipv4Header->getSrcAddress());
    l3AddressInd->setDestAddress(ipv4Header->getDestAddress());
    packet->addTagIfAbsent<HopLimitInd>()->setHopLimit(ipv4Header->getTimeToLive());
}

// ^-^
void Ipv4_ShoreStation::fragmentPostRouting(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::fragmentPostRouting(Packet *packet) \n"; // new added
//    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    const InterfaceEntry *destIE = ift->getInterfaceById(packet->getTag<InterfaceReq>()->getInterfaceId());
    // fill in source address
    if (packet->peekAtFront<Ipv4Header>()->getSrcAddress().isUnspecified()) {
        auto ipv4Header = removeNetworkProtocolHeader<Ipv4Header>(packet);
        ipv4Header->setSrcAddress(destIE->getProtocolData<Ipv4InterfaceData>()->getIPAddress());
        insertNetworkProtocolHeader(packet, Protocol::ipv4, ipv4Header);
    }
    if (datagramPostRoutingHook(packet) == INetfilter::IHook::ACCEPT) fragmentAndSend(packet);
}

void Ipv4_ShoreStation::setComputedCrc(Ptr<Ipv4Header>& ipv4Header)
{
    ASSERT(crcMode == CRC_COMPUTED);
    ipv4Header->setCrc(0);
    MemoryOutputStream ipv4HeaderStream;
    Chunk::serialize(ipv4HeaderStream, ipv4Header);
    // compute the CRC
    uint16_t crc = TcpIpChecksum::checksum(ipv4HeaderStream.getData());
    ipv4Header->setCrc(crc);
}

void Ipv4_ShoreStation::insertCrc(const Ptr<Ipv4Header>& ipv4Header)
{
    CrcMode crcMode = ipv4Header->getCrcMode();
    switch (crcMode) {
        case CRC_DECLARED_CORRECT:
            // if the CRC mode is declared to be correct, then set the CRC to an easily recognizable value
            ipv4Header->setCrc(0xC00D);
            break;
        case CRC_DECLARED_INCORRECT:
            // if the CRC mode is declared to be incorrect, then set the CRC to an easily recognizable value
            ipv4Header->setCrc(0xBAAD);
            break;
        case CRC_COMPUTED: {
            // if the CRC mode is computed, then compute the CRC and set it
            // this computation is delayed after the routing decision, see INetfilter hook
            ipv4Header->setCrc(0x0000); // make sure that the CRC is 0 in the Udp header before computing the CRC
            MemoryOutputStream ipv4HeaderStream;
            Chunk::serialize(ipv4HeaderStream, ipv4Header);
            // compute the CRC
            uint16_t crc = TcpIpChecksum::checksum(ipv4HeaderStream.getData());
            ipv4Header->setCrc(crc);
            break;
        }
        default:
            throw cRuntimeError("Unknown CRC mode: %d", (int)crcMode);
    }
}

// ^-^
void Ipv4_ShoreStation::fragmentAndSend(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::fragmentAndSend(Packet *packet) \n";  // new added
//    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    const InterfaceEntry *destIE = ift->getInterfaceById(packet->getTag<InterfaceReq>()->getInterfaceId());
    Ipv4Address nextHopAddr = getNextHop(packet);
    if (nextHopAddr.isUnspecified()) {
        nextHopAddr = packet->peekAtFront<Ipv4Header>()->getDestAddress();
        packet->addTagIfAbsent<NextHopAddressReq>()->setNextHopAddress(nextHopAddr);
    }
    const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();
    // hop counter check
    if (ipv4Header->getTimeToLive() <= 0) {
        // drop datagram, destruction responsibility in ICMP
        PacketDropDetails details;
        details.setReason(HOP_LIMIT_REACHED);
        emit(packetDroppedSignal, packet, &details);
        EV_WARN << "datagram TTL reached zero, sending ICMP_TIME_EXCEEDED\n";
        sendIcmpError(packet, -1    /*TODO*/, ICMP_TIME_EXCEEDED, 0);
        numDropped++;
        return;
    }
    int mtu = destIE->getMtu();
    // send datagram straight out if it doesn't require fragmentation (note: mtu==0 means infinite mtu)
    if (mtu == 0 || packet->getByteLength() <= mtu) {
//        EV_INFO<<"    --> no fragment\n"; // new added
        if (crcMode == CRC_COMPUTED) {
            auto ipv4Header = removeNetworkProtocolHeader<Ipv4Header>(packet);
            setComputedCrc(ipv4Header);
            insertNetworkProtocolHeader(packet, Protocol::ipv4, ipv4Header);
        }

        // ^- ^ new added !!!
        if(!service_flag) {
            sendDatagramToOutput(packet);   // ^- ^ new added !!!
        }
        else if(service_flag) {
            sendDatagramToOutput_Service(packet);   // ^- ^ new added !!!
        }

        return;
    }

    // if "don't fragment" bit is set, throw datagram away and send ICMP error message
    if (ipv4Header->getDontFragment()) {
        PacketDropDetails details;
        emit(packetDroppedSignal, packet, &details);
        EV_WARN << "datagram larger than MTU and don't fragment bit set, sending ICMP_DESTINATION_UNREACHABLE\n";
        sendIcmpError(packet, -1    /*TODO*/, ICMP_DESTINATION_UNREACHABLE,
                ICMP_DU_FRAGMENTATION_NEEDED);
        numDropped++;
        return;
    }

    // FIXME some IP options should not be copied into each fragment, check their COPY bit
    int headerLength = B(ipv4Header->getHeaderLength()).get();
    int payloadLength = B(packet->getDataLength()).get() - headerLength;
    int fragmentLength = ((mtu - headerLength) / 8) * 8;    // payload only (without header)
    int offsetBase = ipv4Header->getFragmentOffset();
    if (fragmentLength <= 0)
        throw cRuntimeError("Cannot fragment datagram: MTU=%d too small for header size (%d bytes)", mtu, headerLength); // exception and not ICMP because this is likely a simulation configuration error, not something one wants to simulate

    int noOfFragments = (payloadLength + fragmentLength - 1) / fragmentLength;
    EV_DETAIL << "Breaking datagram into " << noOfFragments << " fragments\n";

    // create and send fragments
    std::string fragMsgName = packet->getName();
    fragMsgName += "-frag-";

    int offset = 0;
    while (offset < payloadLength) {
        bool lastFragment = (offset + fragmentLength >= payloadLength);
        // length equal to fragmentLength, except for last fragment;
        int thisFragmentLength = lastFragment ? payloadLength - offset : fragmentLength;

        std::string curFragName = fragMsgName + std::to_string(offset);
        if (lastFragment)
            curFragName += "-last";
        Packet *fragment = new Packet(curFragName.c_str());     //TODO add offset or index to fragment name

        //copy Tags from packet to fragment
        fragment->copyTags(*packet);

        ASSERT(fragment->getByteLength() == 0);
        auto fraghdr = staticPtrCast<Ipv4Header>(ipv4Header->dupShared());
        const auto& fragData = packet->peekDataAt(B(headerLength + offset), B(thisFragmentLength));
        ASSERT(fragData->getChunkLength() == B(thisFragmentLength));
        fragment->insertAtBack(fragData);

        // "more fragments" bit is unchanged in the last fragment, otherwise true
        if (!lastFragment)
            fraghdr->setMoreFragments(true);

        fraghdr->setFragmentOffset(offsetBase + offset);
        fraghdr->setTotalLengthField(B(headerLength + thisFragmentLength));
        if (crcMode == CRC_COMPUTED)
            setComputedCrc(fraghdr);

        fragment->insertAtFront(fraghdr);
        ASSERT(fragment->getByteLength() == headerLength + thisFragmentLength);
        sendDatagramToOutput(fragment);
        offset += thisFragmentLength;
    }

    delete packet;
}

// ^-^  !!! encapsulate Ipv4Header
void Ipv4_ShoreStation::encapsulate(Packet *transportPacket) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::encapsulate(Packet *transportPacket) \n"; // new added
    const auto& ipv4Header = makeShared<Ipv4Header>();

    auto l3AddressReq = transportPacket->removeTag<L3AddressReq>();
    Ipv4Address src = l3AddressReq->getSrcAddress().toIpv4();
    bool nonLocalSrcAddress = l3AddressReq->getNonLocalSrcAddress();
    Ipv4Address dest = l3AddressReq->getDestAddress().toIpv4();
    delete l3AddressReq;

    ipv4Header->setProtocolId((IpProtocolId)ProtocolGroup::ipprotocol.getProtocolNumber(transportPacket->getTag<PacketProtocolTag>()->getProtocol()));

    auto hopLimitReq = transportPacket->removeTagIfPresent<HopLimitReq>();
    short ttl = (hopLimitReq != nullptr) ? hopLimitReq->getHopLimit() : -1;
    delete hopLimitReq;
    bool dontFragment = false;
    if (auto dontFragmentReq = transportPacket->removeTagIfPresent<FragmentationReq>()) {
        dontFragment = dontFragmentReq->getDontFragment();
        delete dontFragmentReq;
    }

    // set source and destination address
    ipv4Header->setDestAddress(dest);

    // when source address was given, use it; otherwise it'll get the address
    // of the outgoing interface after routing
    if (!src.isUnspecified()) {
        if (!nonLocalSrcAddress && rt->getInterfaceByAddress(src) == nullptr)
        // if interface parameter does not match existing interface, do not send datagram
            throw cRuntimeError("Wrong source address %s in (%s)%s: no interface with such address",
                    src.str().c_str(), transportPacket->getClassName(), transportPacket->getFullName());

        ipv4Header->setSrcAddress(src);
    }

    // set other fields
    if (TosReq *tosReq = transportPacket->removeTagIfPresent<TosReq>()) {
        ipv4Header->setTypeOfService(tosReq->getTos());
        delete tosReq;
        if (transportPacket->findTag<DscpReq>())
            throw cRuntimeError("TosReq and DscpReq found together");
        if (transportPacket->findTag<EcnReq>())
            throw cRuntimeError("TosReq and EcnReq found together");
    }
    if (DscpReq *dscpReq = transportPacket->removeTagIfPresent<DscpReq>()) {
        ipv4Header->setDscp(dscpReq->getDifferentiatedServicesCodePoint());
        delete dscpReq;
    }
    if (EcnReq *ecnReq = transportPacket->removeTagIfPresent<EcnReq>()) {
        ipv4Header->setEcn(ecnReq->getExplicitCongestionNotification());
        delete ecnReq;
    }

    ipv4Header->setIdentification(curFragmentId++);
    ipv4Header->setMoreFragments(false);
    ipv4Header->setDontFragment(dontFragment);
    ipv4Header->setFragmentOffset(0);

    if (ttl != -1) {
        ASSERT(ttl > 0);
    }
    else if (ipv4Header->getDestAddress().isLinkLocalMulticast())
        ttl = 1;
    else if (ipv4Header->getDestAddress().isMulticast())
        ttl = defaultMCTimeToLive;
    else
        ttl = defaultTimeToLive;
    ipv4Header->setTimeToLive(ttl);

    if (Ipv4OptionsReq *optReq = transportPacket->removeTagIfPresent<Ipv4OptionsReq>()) {
        for (size_t i = 0; i < optReq->getOptionArraySize(); i++) {
            auto opt = optReq->dropOption(i);
            ipv4Header->addOption(opt);
            ipv4Header->addChunkLength(B(opt->getLength()));
        }
        delete optReq;
    }

    ASSERT(ipv4Header->getChunkLength() <= IPv4_MAX_HEADER_LENGTH);
    ipv4Header->setHeaderLength(ipv4Header->getChunkLength());
    ipv4Header->setTotalLengthField(ipv4Header->getChunkLength() + transportPacket->getDataLength());
    ipv4Header->setCrcMode(crcMode);
    ipv4Header->setCrc(0);
    switch (crcMode) {
        case CRC_DECLARED_CORRECT:
            // if the CRC mode is declared to be correct, then set the CRC to an easily recognizable value
            ipv4Header->setCrc(0xC00D);
            break;
        case CRC_DECLARED_INCORRECT:
            // if the CRC mode is declared to be incorrect, then set the CRC to an easily recognizable value
            ipv4Header->setCrc(0xBAAD);
            break;
        case CRC_COMPUTED: {
            ipv4Header->setCrc(0);
            // crc will be calculated in fragmentAndSend()
            break;
        }
        default:
            throw cRuntimeError("Unknown CRC mode");
    }
    insertNetworkProtocolHeader(transportPacket, Protocol::ipv4, ipv4Header);
    // setting Ipv4 options is currently not supported
}

// ^-^
void Ipv4_ShoreStation::sendDatagramToOutput(Packet *packet) {
//    EV_INFO << "!!! --> Ipv4_ShoreStation::sendDatagramToOutput(Packet *packet) \n"; // new added
//    EV_INFO<<"    --> packet: "<<packet<<"\n";// new added
    const InterfaceEntry *ie = ift->getInterfaceById(packet->getTag<InterfaceReq>()->getInterfaceId());
    auto nextHopAddressReq = packet->removeTag<NextHopAddressReq>();
    Ipv4Address nextHopAddr = nextHopAddressReq->getNextHopAddress().toIpv4();
    delete nextHopAddressReq;
    if (!ie->isBroadcast() || ie->getMacAddress().isUnspecified()) // we can't do ARP
        sendPacketToNIC(packet);
    else {
        MacAddress nextHopMacAddr = resolveNextHopMacAddress(packet, nextHopAddr, ie);
        if (nextHopMacAddr.isUnspecified()) {
            EV_INFO << "Pending " << packet << " to ARP resolution.\n";
            pendingPackets[nextHopAddr].insert(packet);
        }
        else {
            ASSERT2(pendingPackets.find(nextHopAddr) == pendingPackets.end(), "Ipv4-ARP error: nextHopAddr found in ARP table, but Ipv4 queue for nextHopAddr not empty");
//            EV_INFO << "   --> packet before addTagIfAbsent:"<<packet<<"\n"; // new added
            packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(nextHopMacAddr);
//            EV_INFO << "   --> packet after addTagIfAbsent:"<<packet<<"\n"; // new added
//            EV_INFO << "    --> sendPacketToNIC\n"; // new added
            sendPacketToNIC(packet);
        }
    }
}

// ^-^  new added !!! Remove and parse the UdpHeader and Ipv4Header, reconstruct the Ipv4serviceHeader
void Ipv4_ShoreStation::sendDatagramToOutput_Service(Packet *packet) {
    EV_INFO << "!!! --> Ipv4_ShoreStation::sendDatagramToOutput_Service(Packet *packet) \n"; // new added
    EV_INFO<<"    --> packet: "<<packet<<"\n";// new added

    //----------------- Remove and parse the Ipv4Header -----------------
    const auto& ipv4Header = removeNetworkProtocolHeader<Ipv4Header>(packet);
    Ipv4Address srcIpaddr = ipv4Header->getSrcAddress();
    Ipv4Address destIpaddr = ipv4Header->getDestAddress();
    EV_INFO << "    --> packet after removeipv4Header: "<<packet<<"\n";
    EV_INFO << "    --> srcIpaddr: "<<srcIpaddr<<"  destIpaddr: "<<destIpaddr<<"\n";

    //----------------- Remove and parse the UdpHeader -----------------
    const auto& udpHeader = removeNetworkProtocolHeader<UdpHeader>(packet);  // new added
    short srcPort = udpHeader->getSourcePort();
    short destPort = udpHeader->getDestPort();
    EV_INFO << "    --> packet after removeUdpHeader:"<<packet<<"\n";
    EV_INFO << "    --> srcPort: "<< srcPort <<"  destPort: "<< destPort <<endl;  // new added

    //----------------- Inquiry the sid_table and cidshore_table of ShoreStation to get serviceId and MMSI -----------------
    int cidshoreNum = ctshore->getNumCidshores();
    int sidNum = st->getNumSids();
    ServiceId serviceId;
    MacAddress MMSI;
    for(int i=0; i<sidNum; ++i) {
        Ipv4Sid* sid = st->getSid(i);
        if((sid->getIpaddr() == srcIpaddr) && (sid->getPort() == srcPort)) {
            serviceId = sid->getSid();
            EV_INFO<<"    --> srcIpaddr: "<<sid->getIpaddr()<<"  srcPort: "<< srcPort <<"  serviceId: "<<serviceId<< endl;
        }
    }
    for(int i=0; i<cidshoreNum; ++i) {
        Ipv4Cidshore* cidsh = ctshore->getCidshore(i);
        if((cidsh->getIpaddr() == destIpaddr) && (cidsh->getPort() == destPort)) {
            MMSI = cidsh->getMMSI();
            EV_INFO<<"    --> destIpaddr: "<<cidsh->getIpaddr()<<"  destPort: "<< destPort <<"  MMSI: "<< MMSI << endl;
        }
    }
    if(serviceId.isUnspecified()) {
        EV_ERROR<<"    --> no such service, drop this packet !"<<endl;
        return;
    }

    //----------------- reconstruct Ipv4Service package -----------------
    const auto& ipv4ServiceHeader = makeShared<Ipv4ServiceHeader>();
    ipv4ServiceHeader->setServiceId(serviceId);
    insertNetworkProtocolHeader_Service(packet, Protocol::ipv4_service, ipv4ServiceHeader);
    EV_INFO << "    --> ipv4ServiceHeader (" << ipv4ServiceHeader->getChunkLength()<<")" << " [serviceId: "<<ipv4ServiceHeader->getServiceId() << "]"<<endl;
    packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(101);
    const InterfaceEntry *ie = ift->getInterfaceById(101);
    EV_INFO<<"    --> out Interface name: "<< ie->getInterfaceName() <<"\n";// new added
    EV_INFO<<"    --> out Interface Mac Address: "<< ie->getMacAddress() <<"\n";// new added
    MacAddress nextHopMacAddr = MMSI;
    EV_INFO<<"    --> nextHopMacAddr: "<< nextHopMacAddr <<"\n";// new added
    packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(nextHopMacAddr);  // ��packet���Tag: MacAddressReq
    sendPacketToNIC_Service(packet);
}

void Ipv4_ShoreStation::arpResolutionCompleted(IArp::Notification *entry)
{
    if (entry->l3Address.getType() != L3Address::IPv4)
        return;
    auto it = pendingPackets.find(entry->l3Address.toIpv4());
    if (it != pendingPackets.end()) {
        cPacketQueue& packetQueue = it->second;
        EV << "ARP resolution completed for " << entry->l3Address << ". Sending " << packetQueue.getLength()
           << " waiting packets from the queue\n";

        while (!packetQueue.isEmpty()) {
            Packet *packet = check_and_cast<Packet *>(packetQueue.pop());
            EV << "Sending out queued packet " << packet << "\n";
            packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(entry->ie->getInterfaceId());
            packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(entry->macAddress);
            sendPacketToNIC(packet);
        }
        pendingPackets.erase(it);
    }
}

void Ipv4_ShoreStation::arpResolutionTimedOut(IArp::Notification *entry)
{
    if (entry->l3Address.getType() != L3Address::IPv4)
        return;
    auto it = pendingPackets.find(entry->l3Address.toIpv4());
    if (it != pendingPackets.end()) {
        cPacketQueue& packetQueue = it->second;
        EV << "ARP resolution failed for " << entry->l3Address << ",  dropping " << packetQueue.getLength() << " packets\n";
        for (int i = 0; i < packetQueue.getLength(); i++) {
            auto packet = packetQueue.get(i);
            PacketDropDetails details;
            details.setReason(ADDRESS_RESOLUTION_FAILED);
            emit(packetDroppedSignal, packet, &details);
        }
        packetQueue.clear();
        pendingPackets.erase(it);
    }
}

MacAddress Ipv4_ShoreStation::resolveNextHopMacAddress(cPacket *packet, Ipv4Address nextHopAddr, const InterfaceEntry *destIE)
{
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::resolveNextHopMacAddress(cPacket *packet, Ipv4Address nextHopAddr, const InterfaceEntry *destIE)"<<endl; // new added
    if (nextHopAddr.isLimitedBroadcastAddress() || nextHopAddr == destIE->getProtocolData<Ipv4InterfaceData>()->getNetworkBroadcastAddress()) {
//        EV_DETAIL << "destination address is broadcast, sending packet to broadcast MAC address\n";
        return MacAddress::BROADCAST_ADDRESS;
    }

    if (nextHopAddr.isMulticast()) {
        MacAddress macAddr = MacAddress::makeMulticastAddress(nextHopAddr);
        EV_DETAIL << "destination address is multicast, sending packet to MAC address " << macAddr << "\n";
        return macAddr;
    }

    return arp->resolveL3Address(nextHopAddr, destIE);
}

// ^-^
void Ipv4_ShoreStation::sendPacketToNIC(Packet *packet) {
//    EV_INFO << "!!! --> Ipv4_MobileStation::sendPacketToNIC(Packet *packet) \n";  // new added
    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    EV_INFO << "Sending " << packet << " to output interface = " << ift->getInterfaceById(packet->getTag<InterfaceReq>()->getInterfaceId())->getInterfaceName() << ".\n";
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ipv4);
    packet->addTagIfAbsent<DispatchProtocolInd>()->setProtocol(&Protocol::ipv4);
    delete packet->removeTagIfPresent<DispatchProtocolReq>();
    ASSERT(packet->findTag<InterfaceReq>() != nullptr);
    send(packet, "queueOut");
}

// new added  !!!  Send message to the Maritime_MTC NIC
void Ipv4_ShoreStation::sendPacketToNIC_Service(Packet *packet) {
    EV_INFO << "!!! --> Ipv4_ShoreStation::sendPacketToNIC_Service(Packet *packet) \n";
    EV_INFO<<"    --> packet: "<<packet<<"\n"; // new added
    EV_INFO << "Sending " << packet << " to output interface = " << ift->getInterfaceById(101)->getInterfaceName() << ".\n";

    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ipv4);
    packet->addTagIfAbsent<DispatchProtocolInd>()->setProtocol(&Protocol::ipv4);
    delete packet->removeTagIfPresent<DispatchProtocolReq>();
    ASSERT(packet->findTag<InterfaceReq>() != nullptr);
    send(packet, "queueOut");
}

// NetFilter:
void Ipv4_ShoreStation::registerHook(int priority, INetfilter::IHook *hook)
{
    Enter_Method("registerHook()");
    NetfilterBase::registerHook(priority, hook);
}

void Ipv4_ShoreStation::unregisterHook(INetfilter::IHook *hook)
{
    Enter_Method("unregisterHook()");
    NetfilterBase::unregisterHook(hook);
}

void Ipv4_ShoreStation::dropQueuedDatagram(const Packet *packet)
{
    Enter_Method("dropQueuedDatagram()");
    for (auto iter = queuedDatagramsForHooks.begin(); iter != queuedDatagramsForHooks.end(); iter++) {
        if (iter->packet == packet) {
            delete packet;
            queuedDatagramsForHooks.erase(iter);
            return;
        }
    }
}

void Ipv4_ShoreStation::reinjectQueuedDatagram(const Packet *packet)
{
    Enter_Method("reinjectDatagram()");
    for (auto iter = queuedDatagramsForHooks.begin(); iter != queuedDatagramsForHooks.end(); iter++) {
        if (iter->packet == packet) {
            auto *qPacket = iter->packet;
            take(qPacket);
            switch (iter->hookType) {
                case INetfilter::IHook::LOCALOUT:
                    datagramLocalOut(qPacket);
                    break;

                case INetfilter::IHook::PREROUTING:
                    preroutingFinish(qPacket);
                    break;

                case INetfilter::IHook::POSTROUTING:
                    fragmentAndSend(qPacket);
                    break;

                case INetfilter::IHook::LOCALIN:
                    reassembleAndDeliverFinish(qPacket);
                    break;

                case INetfilter::IHook::FORWARD:
                    routeUnicastPacketFinish(qPacket);
                    break;

                default:
                    throw cRuntimeError("Unknown hook ID: %d", (int)(iter->hookType));
                    break;
            }
            queuedDatagramsForHooks.erase(iter);
            return;
        }
    }
}

// ^-^
INetfilter::IHook::Result Ipv4_ShoreStation::datagramPreRoutingHook(Packet *packet) {
//    EV_INFO<<"   !--> Ipv4_ShoreStation::datagramPreRoutingHook(Packet *packet) \n"; // new added
    for (auto & elem : hooks) {
        IHook::Result r = elem.second->datagramPreRoutingHook(packet);
        switch (r) {
            case INetfilter::IHook::ACCEPT:
                break;    // continue iteration

            case INetfilter::IHook::DROP:
                delete packet;
                return r;

            case INetfilter::IHook::QUEUE:
                queuedDatagramsForHooks.push_back(QueuedDatagramForHook(packet, INetfilter::IHook::PREROUTING));
                return r;

            case INetfilter::IHook::STOLEN:
                return r;

            default:
                throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return INetfilter::IHook::ACCEPT;
}

INetfilter::IHook::Result Ipv4_ShoreStation::datagramForwardHook(Packet *packet)
{
    for (auto & elem : hooks) {
        IHook::Result r = elem.second->datagramForwardHook(packet);
        switch (r) {
            case INetfilter::IHook::ACCEPT:
                break;    // continue iteration

            case INetfilter::IHook::DROP:
                delete packet;
                return r;

            case INetfilter::IHook::QUEUE:
                queuedDatagramsForHooks.push_back(QueuedDatagramForHook(packet, INetfilter::IHook::FORWARD));
                return r;

            case INetfilter::IHook::STOLEN:
                return r;

            default:
                throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return INetfilter::IHook::ACCEPT;
}

// ^-^
INetfilter::IHook::Result Ipv4_ShoreStation::datagramPostRoutingHook(Packet *packet) {
//    EV_INFO<<"   !--> Ipv4_ShoreStation::datagramPostRoutingHook(Packet *packet)\n";  //new added
    for (auto & elem : hooks) {
        IHook::Result r = elem.second->datagramPostRoutingHook(packet);
        switch (r) {
            case INetfilter::IHook::ACCEPT:
                break;    // continue iteration

            case INetfilter::IHook::DROP:
                delete packet;
                return r;

            case INetfilter::IHook::QUEUE:
                queuedDatagramsForHooks.push_back(QueuedDatagramForHook(packet, INetfilter::IHook::POSTROUTING));
                return r;

            case INetfilter::IHook::STOLEN:
                return r;

            default:
                throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return INetfilter::IHook::ACCEPT;
}

void Ipv4_ShoreStation::handleStartOperation(LifecycleOperation *operation)
{
    start();
}

void Ipv4_ShoreStation::handleStopOperation(LifecycleOperation *operation)
{
    // TODO: stop should send and wait pending packets
    stop();
}

void Ipv4_ShoreStation::handleCrashOperation(LifecycleOperation *operation)
{
    stop();
}

void Ipv4_ShoreStation::start()
{
}

void Ipv4_ShoreStation::stop()
{
    flush();
    for (auto it : socketIdToSocketDescriptor)
        delete it.second;
    socketIdToSocketDescriptor.clear();
}

void Ipv4_ShoreStation::flush()
{
    EV_DEBUG << "Ipv4::flush(): pending packets:\n";
    for (auto & elem : pendingPackets) {
        EV_DEBUG << "Ipv4::flush():    " << elem.first << ": " << elem.second.str() << endl;
        elem.second.clear();
    }
    pendingPackets.clear();

    EV_DEBUG << "Ipv4::flush(): packets in hooks: " << queuedDatagramsForHooks.size() << endl;
    for (auto & elem : queuedDatagramsForHooks) {
        delete elem.packet;
    }
    queuedDatagramsForHooks.clear();

    fragbuf.flush();
}

INetfilter::IHook::Result Ipv4_ShoreStation::datagramLocalInHook(Packet *packet)
{
    for (auto & elem : hooks) {
        IHook::Result r = elem.second->datagramLocalInHook(packet);
        switch (r) {
            case INetfilter::IHook::ACCEPT:
                break;    // continue iteration

            case INetfilter::IHook::DROP:
                delete packet;
                return r;

            case INetfilter::IHook::QUEUE: {
                if (packet->getOwner() != this)
                    throw cRuntimeError("Model error: netfilter hook changed the owner of queued datagram '%s'", packet->getFullName());
                queuedDatagramsForHooks.push_back(QueuedDatagramForHook(packet, INetfilter::IHook::LOCALIN));
                return r;
            }

            case INetfilter::IHook::STOLEN:
                return r;

            default:
                throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return INetfilter::IHook::ACCEPT;
}

// ^-^
INetfilter::IHook::Result Ipv4_ShoreStation::datagramLocalOutHook(Packet *packet) {
//    EV_INFO<<"!!! --> Ipv4_ShoreStation::datagramLocalOutHook(Packet *packet) \n";
    for (auto & elem : hooks) {
        IHook::Result r = elem.second->datagramLocalOutHook(packet);
        switch (r) {
            case INetfilter::IHook::ACCEPT:
                break;    // continue iteration

            case INetfilter::IHook::DROP:
                delete packet;
                return r;

            case INetfilter::IHook::QUEUE:
                queuedDatagramsForHooks.push_back(QueuedDatagramForHook(packet, INetfilter::IHook::LOCALOUT));
                return r;

            case INetfilter::IHook::STOLEN:
                return r;

            default:
                throw cRuntimeError("Unknown Hook::Result value: %d", (int)r);
        }
    }
    return INetfilter::IHook::ACCEPT;
}

void Ipv4_ShoreStation::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details)
{
    Enter_Method_Silent();

    if (signalID == IArp::arpResolutionCompletedSignal) {
        arpResolutionCompleted(check_and_cast<IArp::Notification *>(obj));
    }
    if (signalID == IArp::arpResolutionFailedSignal) {
        arpResolutionTimedOut(check_and_cast<IArp::Notification *>(obj));
    }
}

void Ipv4_ShoreStation::sendIcmpError(Packet *origPacket, int inputInterfaceId, IcmpType type, IcmpCode code)
{
    icmp->sendErrorMessage(origPacket, inputInterfaceId, type, code);
}

} // namespace inet
