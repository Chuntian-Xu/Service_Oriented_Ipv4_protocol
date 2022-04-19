#include "inet/applications/common/SocketTag_m.h"
#include "inet/applications/udpapp/UdpSocketIo_1.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace inet {

Define_Module(UdpSocketIo_1);

void UdpSocketIo_1::initialize(int stage)
{
    EV_INFO<<"!!! --> UdpSocketIo_1::initialize(int stage)"<<endl;  // new added
    ApplicationBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        dontFragment = par("dontFragment");
        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);
    }
}

void UdpSocketIo_1::handleMessageWhenUp(cMessage *message)
{
    EV_INFO<<"!!! --> UdpSocketIo_1::handleMessageWhenUp(cMessage *message)"<<endl; // new added
    if (socket.belongsToSocket(message)){
        EV_INFO<<"    --> belongs To socket"<<endl;
        socket.processMessage(message);
    }
    else if(socket_receive.belongsToSocket(message)){
        EV_INFO<<"    --> belongs To socket_receive"<<endl;
        socket_receive.processMessage(message);
    }
    else {
        auto packet = check_and_cast<Packet *>(message);
        if (dontFragment)
            packet->addTagIfAbsent<FragmentationReq>()->setDontFragment(true);
        socket.send(packet);
        numSent++;
        emit(packetSentSignal, packet);
        CreatNewSocketAndReceive();
    }
}

void UdpSocketIo_1::finish()
{
    EV_INFO<<"!!! --> UdpSocketIo_1::finish()"<<endl; // new added
    recordScalar("packets sent", numSent);
    recordScalar("packets received", numReceived);
    ApplicationBase::finish();
}

void UdpSocketIo_1::refreshDisplay() const
{
//    EV_INFO<<"!!! --> refreshDisplay() const"<<endl; // new added
    ApplicationBase::refreshDisplay();
    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void UdpSocketIo_1::setSocketOptions(UdpSocket *socket)
{
    EV_INFO<<"!!! --> setSocketOptions()"<<endl; // new added
    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        socket->setTimeToLive(timeToLive);

    int dscp = par("dscp");
    if (dscp != -1)
        socket->setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        socket->setTos(tos);

    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        InterfaceEntry *ie = ift->findInterfaceByName(multicastInterface);
        if (!ie)
            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
        socket->setMulticastOutputInterface(ie->getInterfaceId());
    }

    bool receiveBroadcast = par("receiveBroadcast");
    if (receiveBroadcast)
        socket->setBroadcast(true);

    bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
    if (joinLocalMulticastGroups) {
        MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
        socket->joinLocalMulticastGroups(mgl);
    }
    socket->setCallback(this);
}

void UdpSocketIo_1::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    EV_INFO<< "!!! --> UdpSocketIo_1::socketDataArrived(UdpSocket *socket, Packet *packet)" << endl;
    emit(packetReceivedSignal, packet);
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << endl;
    numReceived++;
    delete packet->removeTag<SocketInd>();
	send(packet, "trafficOut");  // new changed
}

// new added
void UdpSocketIo_1::CreatNewSocketAndReceive(){
    EV_INFO<< "!!! --> UdpSocketIo_1::CreatNewSocketAndReceive(Packet *packet)" << endl;
    socket_receive.setOutputGate(gate("socketOut"));
    setSocketOptions(&socket_receive); // new changed
    const char *localAddress_receive = par("localAddress_receive");
    u_short localPort_receive = par("localPort_receive");
    EV_INFO<<"    --> localPort_receive: "<< localPort_receive << ", localAddress_receive " << localAddress_receive <<endl;
    socket_receive.bind(*localAddress_receive ? L3AddressResolver().resolve(localAddress_receive) : L3Address(), localPort_receive);
}

void UdpSocketIo_1::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_INFO<< "!!! --> UdpSocketIo_1::socketErrorArrived(UdpSocket *socket, Indication *indication)" << endl;  // new added
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void UdpSocketIo_1::socketClosed(UdpSocket *socket)
{
    EV_INFO<< "!!! --> UdpSocketIo_1::socketClosed(UdpSocket *socket)" << endl;  // new added
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void UdpSocketIo_1::handleStartOperation(LifecycleOperation *operation)
{
    EV_INFO<< "!!! --> UdpSocketIo_1::handleStartOperation(LifecycleOperation *operation)" << endl;  // new added
    socket.setOutputGate(gate("socketOut")); // new changed
    setSocketOptions(&socket); // new changed
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), par("localPort"));
    const char *destAddrs = par("destAddress");
    socket.connect(*destAddrs ? L3AddressResolver().resolve(destAddrs) : L3Address(), par("destPort"));
}

void UdpSocketIo_1::handleStopOperation(LifecycleOperation *operation)
{
    EV_INFO<< "!!! --> handleStopOperation(LifecycleOperation *operation)" << endl;  // new added
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void UdpSocketIo_1::handleCrashOperation(LifecycleOperation *operation)
{
    EV_INFO<< "!!! --> handleCrashOperation(LifecycleOperation *operation)" << endl;  // new added
    socket.destroy();
}

} // namespace inet
