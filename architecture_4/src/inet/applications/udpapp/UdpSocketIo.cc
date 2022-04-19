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

#include "inet/applications/common/SocketTag_m.h"
#include "inet/applications/udpapp/UdpSocketIo.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace inet {

Define_Module(UdpSocketIo);

void UdpSocketIo::initialize(int stage)
{
    EV_INFO<<"!!! --> UdpSocketIo::initialize(int stage)"<<endl;  // new added
    ApplicationBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        dontFragment = par("dontFragment");
        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);
    }
}

void UdpSocketIo::handleMessageWhenUp(cMessage *message)
{
    EV_INFO<<"!!! --> UdpSocketIo::handleMessageWhenUp(cMessage *message)"<<endl; // new added
    if (socket.belongsToSocket(message))
        socket.processMessage(message);
    else {
        auto packet = check_and_cast<Packet *>(message);
        if (dontFragment)
            packet->addTagIfAbsent<FragmentationReq>()->setDontFragment(true);
        socket.send(packet);
        numSent++;
        emit(packetSentSignal, packet);
    }
}

void UdpSocketIo::finish()
{
    EV_INFO<<"!!! --> UdpSocketIo::finish()"<<endl; // new added
    recordScalar("packets sent", numSent);
    recordScalar("packets received", numReceived);
    ApplicationBase::finish();
}

void UdpSocketIo::refreshDisplay() const
{
//    EV_INFO<<"!!! --> refreshDisplay() const"<<endl; // new added
    ApplicationBase::refreshDisplay();
    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void UdpSocketIo::setSocketOptions()
{
    EV_INFO<<"!!! --> setSocketOptions()"<<endl; // new added
    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        socket.setTimeToLive(timeToLive);

    int dscp = par("dscp");
    if (dscp != -1)
        socket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        InterfaceEntry *ie = ift->findInterfaceByName(multicastInterface);
        if (!ie)
            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
        socket.setMulticastOutputInterface(ie->getInterfaceId());
    }

    bool receiveBroadcast = par("receiveBroadcast");
    if (receiveBroadcast)
        socket.setBroadcast(true);

    bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
    if (joinLocalMulticastGroups) {
        MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
        socket.joinLocalMulticastGroups(mgl);
    }
    socket.setCallback(this);
}

void UdpSocketIo::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    EV_INFO<< "!!! --> UdpSocketIo::socketDataArrived(UdpSocket *socket, Packet *packet)" << endl;
    emit(packetReceivedSignal, packet);
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet) << endl;
    numReceived++;
    delete packet->removeTag<SocketInd>();
//    send(packet, "trafficOut");  // new changed
    CreatNewSocketAndReply(packet);  // new changed
}

// new added
void UdpSocketIo::CreatNewSocketAndReply(Packet *packet){
    EV_INFO<< "!!! --> UdpSocketIo::CreatNewSocketAndReply(Packet *packet)" << endl;
    socket_reply.setOutputGate(gate("socketOut"));
    const char *localAddress_ = par("localAddress_");
    u_short localPort = par("localPort");
    u_short localPort_reply = localPort;
    while(localPort_reply == localPort){
        localPort_reply = (rand()%(65535-49152+1))+ 49152;
    }
    EV_INFO<<"    --> localPort_reply: "<< localPort_reply <<endl;
    socket_reply.bind(*localAddress_ ? L3AddressResolver().resolve(localAddress_) : L3Address(), localPort_reply);
    const char *destAddrs = par("destAddress");
    u_short destPort = par("destPort");
    EV_INFO<<"    --> destAddrs: "<< destAddrs <<endl;
    EV_INFO<<"    --> destPort: "<< destPort <<endl;
    packet->clearTags();
    packet->trim();
    packet->setName("UdpAppData-0-reply");
    socket_reply.sendTo(packet, *destAddrs ? L3AddressResolver().resolve(destAddrs) : L3Address(), destPort);
}

void UdpSocketIo::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_INFO<< "!!! --> UdpSocketIo::socketErrorArrived(UdpSocket *socket, Indication *indication)" << endl;  // new added
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void UdpSocketIo::socketClosed(UdpSocket *socket)
{
    EV_INFO<< "!!! --> UdpSocketIo::socketClosed(UdpSocket *socket)" << endl;  // new added
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void UdpSocketIo::handleStartOperation(LifecycleOperation *operation)
{
    EV_INFO<< "!!! --> UdpSocketIo::handleStartOperation(LifecycleOperation *operation)" << endl;  // new added
    socket.setOutputGate(gate("socketOut")); // new changed
    setSocketOptions(); // new changed
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), par("localPort"));
    const char *destAddrs = par("destAddress");
    socket.connect(*destAddrs ? L3AddressResolver().resolve(destAddrs) : L3Address(), par("destPort"));
}

void UdpSocketIo::handleStopOperation(LifecycleOperation *operation)
{
    EV_INFO<< "!!! --> handleStopOperation(LifecycleOperation *operation)" << endl;  // new added
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void UdpSocketIo::handleCrashOperation(LifecycleOperation *operation)
{
    EV_INFO<< "!!! --> handleCrashOperation(LifecycleOperation *operation)" << endl;  // new added
    socket.destroy();
}

} // namespace inet
