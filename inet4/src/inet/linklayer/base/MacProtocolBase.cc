// src/inet/linklayer/base/MacProtocolBase.cc

#include "inet/common/IInterfaceRegistrationListener.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/base/MacProtocolBase.h"

namespace inet {

MacProtocolBase::MacProtocolBase()
{
}

MacProtocolBase::~MacProtocolBase()
{
    delete currentTxFrame;
    if (hostModule)
        hostModule->unsubscribe(interfaceDeletedSignal, this);
}

MacAddress MacProtocolBase::parseMacAddressParameter(const char *addrstr)
{
    MacAddress address;

    if (!strcmp(addrstr, "auto"))
        // assign automatic address
        address = MacAddress::generateAutoAddress();
    else
        address.setAddress(addrstr);

    return address;
}

void MacProtocolBase::initialize(int stage)
{
    LayeredProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        upperLayerInGateId = findGate("upperLayerIn");
        upperLayerOutGateId = findGate("upperLayerOut");
        lowerLayerInGateId = findGate("lowerLayerIn");
        lowerLayerOutGateId = findGate("lowerLayerOut");
        hostModule = findContainingNode(this);
        if (hostModule)
            hostModule->subscribe(interfaceDeletedSignal, this);
    }
    else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION)
        registerInterface();
}

void MacProtocolBase::registerInterface()
{
    ASSERT(interfaceEntry == nullptr);
    interfaceEntry = getContainingNicModule(this);
    configureInterfaceEntry();
}

void MacProtocolBase::sendUp(cMessage *message)
{
    if (message->isPacket())
        emit(packetSentToUpperSignal, message);
    send(message, upperLayerOutGateId);
}

// ^-^
void MacProtocolBase::sendDown(cMessage *message) {
    EV_INFO<<"!!! --> MacProtocolBase::sendDown(cMessage *message)\n"; // new added
    if (message->isPacket())
        emit(packetSentToLowerSignal, message);
    send(message, lowerLayerOutGateId);
}

bool MacProtocolBase::isUpperMessage(cMessage *message)
{
    return message->getArrivalGateId() == upperLayerInGateId;
}

bool MacProtocolBase::isLowerMessage(cMessage *message)
{
    return message->getArrivalGateId() == lowerLayerInGateId;
}

void MacProtocolBase::deleteCurrentTxFrame()
{
    delete currentTxFrame;
    currentTxFrame = nullptr;
}

void MacProtocolBase::dropCurrentTxFrame(PacketDropDetails& details)
{
    emit(packetDroppedSignal, currentTxFrame, &details);
    delete currentTxFrame;
    currentTxFrame = nullptr;
}

void MacProtocolBase::popTxQueue()
{
    if (currentTxFrame != nullptr)
        throw cRuntimeError("Model error: incomplete transmission exists");
    ASSERT(txQueue != nullptr);
    currentTxFrame = txQueue->popPacket();
    take(currentTxFrame);
}

void MacProtocolBase::flushQueue(PacketDropDetails& details)
{
    // code would look slightly nicer with a pop() function that returns nullptr if empty
    if (txQueue)
        while (!txQueue->isEmpty()) {
            auto packet = txQueue->popPacket();
            emit(packetDroppedSignal, packet, &details); //FIXME this signal lumps together packets from the network and packets from higher layers! separate them
            delete packet;
        }
}

void MacProtocolBase::clearQueue()
{
    if (txQueue)
        while (!txQueue->isEmpty())
            delete txQueue->popPacket();
}

void MacProtocolBase::handleMessageWhenDown(cMessage *msg)
{
    if (!msg->isSelfMessage() && msg->getArrivalGateId() == lowerLayerInGateId) {
        EV << "Interface is turned off, dropping packet\n";
        delete msg;
    }
    else
        LayeredProtocolBase::handleMessageWhenDown(msg);
}

void MacProtocolBase::handleStartOperation(LifecycleOperation *operation)
{
    interfaceEntry->setState(InterfaceEntry::State::UP);
    interfaceEntry->setCarrier(true);
}

void MacProtocolBase::handleStopOperation(LifecycleOperation *operation)
{
    PacketDropDetails details;
    details.setReason(INTERFACE_DOWN);
    if (currentTxFrame)
        dropCurrentTxFrame(details);
    flushQueue(details);
    interfaceEntry->setCarrier(false);
    interfaceEntry->setState(InterfaceEntry::State::DOWN);
}

void MacProtocolBase::handleCrashOperation(LifecycleOperation *operation)
{
    deleteCurrentTxFrame();
    clearQueue();
    interfaceEntry->setCarrier(false);
    interfaceEntry->setState(InterfaceEntry::State::DOWN);
}

void MacProtocolBase::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details)
{
    if (signalID == interfaceDeletedSignal) {
        if (interfaceEntry == check_and_cast<const InterfaceEntry *>(obj))
            interfaceEntry = nullptr;
    }
}

} // namespace inet

