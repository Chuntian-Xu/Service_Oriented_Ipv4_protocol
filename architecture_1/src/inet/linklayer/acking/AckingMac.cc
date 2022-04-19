// inet/src/inet/linklayer/acking/AckingMac.cc

#include <stdio.h>
#include <string.h>

#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolGroup.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/linklayer/acking/AckingMac.h"
#include "inet/linklayer/acking/AckingMacHeader_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

namespace inet {

using namespace inet::physicallayer;

Define_Module(AckingMac);

AckingMac::AckingMac()
{
}

AckingMac::~AckingMac()
{
    cancelAndDelete(ackTimeoutMsg);
}

void AckingMac::initialize(int stage)
{
    MacProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        bitrate = par("bitrate");
        headerLength = par("headerLength");
        promiscuous = par("promiscuous");
        fullDuplex = par("fullDuplex");
        useAck = par("useAck");
        ackTimeout = par("ackTimeout");

        cModule *radioModule = gate("lowerLayerOut")->getPathEndGate()->getOwnerModule();
        radioModule->subscribe(IRadio::transmissionStateChangedSignal, this);
        radio = check_and_cast<IRadio *>(radioModule);
        transmissionState = IRadio::TRANSMISSION_STATE_UNDEFINED;

        txQueue = check_and_cast<queueing::IPacketQueue *>(getSubmodule("queue"));
    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        radio->setRadioMode(fullDuplex ? IRadio::RADIO_MODE_TRANSCEIVER : IRadio::RADIO_MODE_RECEIVER);
        if (useAck)
            ackTimeoutMsg = new cMessage("link-break");
        if (!txQueue->isEmpty()) {
            popTxQueue();
            startTransmitting();
        }
    }
}

void AckingMac::configureInterfaceEntry()
{
    MacAddress address = parseMacAddressParameter(par("address"));

    // data rate
    interfaceEntry->setDatarate(bitrate);

    // generate a link-layer address to be used as interface token for IPv6
    interfaceEntry->setMacAddress(address);
    interfaceEntry->setInterfaceToken(address.formInterfaceIdentifier());

    // MTU: typical values are 576 (Internet de facto), 1500 (Ethernet-friendly),
    // 4000 (on some point-to-point links), 4470 (Cisco routers default, FDDI compatible)
    interfaceEntry->setMtu(par("mtu"));

    // capabilities
    interfaceEntry->setMulticast(true);
    interfaceEntry->setBroadcast(true);
}

void AckingMac::receiveSignal(cComponent *source, simsignal_t signalID, intval_t value, cObject *details)
{
    Enter_Method_Silent();
    if (signalID == IRadio::transmissionStateChangedSignal) {
        IRadio::TransmissionState newRadioTransmissionState = static_cast<IRadio::TransmissionState>(value);
        if (transmissionState == IRadio::TRANSMISSION_STATE_TRANSMITTING && newRadioTransmissionState == IRadio::TRANSMISSION_STATE_IDLE) {
            radio->setRadioMode(fullDuplex ? IRadio::RADIO_MODE_TRANSCEIVER : IRadio::RADIO_MODE_RECEIVER);
            if (!currentTxFrame && !txQueue->isEmpty()) {
                popTxQueue();
                startTransmitting();
            }
        }
        transmissionState = newRadioTransmissionState;
    }
}

void AckingMac::startTransmitting()
{
    EV_INFO<<"!!! --> void AckingMac::startTransmitting()"<<endl;  // new added
    // if there's any control info, remove it; then encapsulate the packet
    MacAddress dest = currentTxFrame->getTag<MacAddressReq>()->getDestAddress();
    Packet *msg = currentTxFrame;
    if (useAck && !dest.isBroadcast() && !dest.isMulticast() && !dest.isUnspecified()) {    // unicast
        msg = currentTxFrame->dup();
        scheduleAt(simTime() + ackTimeout, ackTimeoutMsg);
    }
    else
        currentTxFrame = nullptr;

    encapsulate(msg);

    // send
    EV << "Starting transmission of " << msg << endl;
    EV_INFO<<"    --> dest mac address: "<<dest<<endl;  // new added
    radio->setRadioMode(fullDuplex ? IRadio::RADIO_MODE_TRANSCEIVER : IRadio::RADIO_MODE_TRANSMITTER);
    sendDown(msg);
}

void AckingMac::handleUpperPacket(Packet *packet)
{
    EV_INFO << "!!! --> void AckingMac::handleUpperPacket(Packet *packet)\n";  // new added
    EV << "Received " << packet << " for transmission\n";
    txQueue->pushPacket(packet);
    if (currentTxFrame || radio->getTransmissionState() == IRadio::TRANSMISSION_STATE_TRANSMITTING)
        EV << "Delaying transmission of " << packet << ".\n";
    else if (!txQueue->isEmpty()){
        popTxQueue();
        startTransmitting();
    }
}

void AckingMac::handleLowerPacket(Packet *packet)
{
    EV_INFO<<"!!! --> AckingMac::handleLowerPacket(Packet *packet)"<<endl;  // new added
    auto macHeader = packet->peekAtFront<AckingMacHeader>();
    if (packet->hasBitError()) {
        EV << "Received frame '" << packet->getName() << "' contains bit errors or collision, dropping it\n";
        PacketDropDetails details;
        details.setReason(INCORRECTLY_RECEIVED);
        emit(packetDroppedSignal, packet, &details);
        delete packet;
        return;
    }

    if (!dropFrameNotForUs(packet)) {
        // send Ack if needed
        auto dest = macHeader->getDest();
        bool needsAck = !(dest.isBroadcast() || dest.isMulticast() || dest.isUnspecified()); // same condition as in sender
        if (needsAck) {
            int senderModuleId = macHeader->getSrcModuleId();
            AckingMac *senderMac = check_and_cast<AckingMac *>(getSimulation()->getModule(senderModuleId));
            if (senderMac->useAck)
                senderMac->acked(packet);
        }

        // decapsulate and attach control info
        decapsulate(packet);
        EV << "Passing up contained packet '" << packet->getName() << "' to higher layer\n";
        sendUp(packet);
    }
}

void AckingMac::handleSelfMessage(cMessage *message)
{
    if (message == ackTimeoutMsg) {
        EV_DETAIL << "AckingMac: timeout: " << currentTxFrame->getFullName() << " is lost\n";
        // packet lost
        emit(linkBrokenSignal, currentTxFrame);
        PacketDropDetails details;
        details.setReason(OTHER_PACKET_DROP);
        dropCurrentTxFrame(details);
        if (!txQueue->isEmpty()) {
            popTxQueue();
            startTransmitting();
        }
    }
    else {
        MacProtocolBase::handleSelfMessage(message);
    }
}

void AckingMac::acked(Packet *frame)
{
    Enter_Method_Silent();
    ASSERT(useAck);

    if (currentTxFrame == nullptr)
        throw cRuntimeError("Unexpected ACK received");

    EV_DEBUG << "AckingMac::acked(" << frame->getFullName() << ") is accepted\n";
    cancelEvent(ackTimeoutMsg);
        deleteCurrentTxFrame();
        if (!txQueue->isEmpty()) {
            popTxQueue();
            startTransmitting();
        }
}

void AckingMac::encapsulate(Packet *packet)
{
    EV_INFO<<"!!! --> AckingMac::encapsulate(Packet *packet)"<<endl;  // new added
    auto macHeader = makeShared<AckingMacHeader>();
    macHeader->setChunkLength(B(headerLength));
    auto macAddressReq = packet->getTag<MacAddressReq>();
    macHeader->setSrc(macAddressReq->getSrcAddress());
    macHeader->setDest(macAddressReq->getDestAddress());
    MacAddress dest = macAddressReq->getDestAddress();
    if (dest.isBroadcast() || dest.isMulticast() || dest.isUnspecified())
        macHeader->setSrcModuleId(-1);
    else
        macHeader->setSrcModuleId(getId());
    macHeader->setNetworkProtocol(ProtocolGroup::ethertype.getProtocolNumber(packet->getTag<PacketProtocolTag>()->getProtocol()));
    packet->insertAtFront(macHeader);
    auto macAddressInd = packet->addTagIfAbsent<MacAddressInd>();
    macAddressInd->setSrcAddress(macHeader->getSrc());
    macAddressInd->setDestAddress(macHeader->getDest());
    packet->getTag<PacketProtocolTag>()->setProtocol(&Protocol::ackingMac);
}

bool AckingMac::dropFrameNotForUs(Packet *packet)
{
    EV_INFO<<"!!! --> AckingMac::dropFrameNotForUs(Packet *packet)"<<endl;  // new added
    auto macHeader = packet->peekAtFront<AckingMacHeader>();
    // Current implementation does not support the configuration of multicast
    // MAC address groups. We rather accept all multicast frames (just like they were
    // broadcasts) and pass it up to the higher layer where they will be dropped
    // if not needed.
    // All frames must be passed to the upper layer if the interface is
    // in promiscuous mode.

    if (macHeader->getDest().equals(interfaceEntry->getMacAddress())){
        EV << "Frame '" << packet->getName() << "' destined to us, do not discarding\n";
        return false;
    }

    if (macHeader->getDest().isBroadcast()){
        EV << "Frame '" << packet->getName() << "' destined to us, do not discarding\n";
        return false;
    }

    if (promiscuous || macHeader->getDest().isMulticast()){
        EV << "Frame '" << packet->getName() << "' destined to us, do not discarding\n";
        return false;
    }

    EV << "Frame '" << packet->getName() << "' not destined to us, discarding\n";
    PacketDropDetails details;
    details.setReason(NOT_ADDRESSED_TO_US);
    emit(packetDroppedSignal, packet, &details);
    delete packet;
    return true;
}

void AckingMac::decapsulate(Packet *packet)
{
    EV_INFO<<"!!! --> AckingMac::decapsulate(Packet *packet)"<<endl;  // new added
    const auto& macHeader = packet->popAtFront<AckingMacHeader>();
    auto macAddressInd = packet->addTagIfAbsent<MacAddressInd>();
    macAddressInd->setSrcAddress(macHeader->getSrc());
    macAddressInd->setDestAddress(macHeader->getDest());
    packet->addTagIfAbsent<InterfaceInd>()->setInterfaceId(interfaceEntry->getInterfaceId());
    auto payloadProtocol = ProtocolGroup::ethertype.getProtocol(macHeader->getNetworkProtocol());
    packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(payloadProtocol);
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(payloadProtocol);
}

} // namespace inet

