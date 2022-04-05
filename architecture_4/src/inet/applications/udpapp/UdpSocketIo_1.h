#ifndef __INET_UDPSOCKETIO_1_H
#define __INET_UDPSOCKETIO_1_H

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

namespace inet {

class INET_API UdpSocketIo_1 : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    bool dontFragment = false;
    UdpSocket socket;
    UdpSocket socket_receive;  // new added
    int numSent = 0;
    int numReceived = 0;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *message) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void setSocketOptions(UdpSocket *socket);

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void CreatNewSocketAndReceive();  // new added
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
};

} // namespace inet

#endif // ifndef __INET_UDPSOCKETIO_1_H
