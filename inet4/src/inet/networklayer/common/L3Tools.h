// src/inet/networklayer/common/L3Tools.h 

#ifndef __INET_L3TOOLS_H
#define __INET_L3TOOLS_H

#include "inet/common/ProtocolTools.h"
#include "inet/networklayer/contract/NetworkHeaderBase_m.h"
#include "inet/networklayer/contract/NetworkServiceHeaderBase_m.h"

namespace inet {

INET_API const Protocol *findNetworkProtocol(Packet *packet);
INET_API const Protocol& getNetworkProtocol(Packet *packet);

INET_API const Ptr<const NetworkHeaderBase> findNetworkProtocolHeader(Packet *packet);
INET_API const Ptr<const NetworkHeaderBase> getNetworkProtocolHeader(Packet *packet);

INET_API const Ptr<const NetworkHeaderBase> peekNetworkProtocolHeader(const Packet *packet, const Protocol& protocol);

INET_API void insertNetworkProtocolHeader(Packet *packet, const Protocol& protocol, const Ptr<NetworkHeaderBase>& header);
INET_API void insertNetworkProtocolHeader_Service(Packet *packet, const Protocol& protocol, const Ptr<NetworkServiceHeaderBase>& header);

template <typename T>
const Ptr<T> removeNetworkProtocolHeader(Packet *packet)
{
    delete packet->removeTagIfPresent<NetworkProtocolInd>();
    return removeProtocolHeader<T>(packet);
}

INET_API const Ptr<NetworkHeaderBase> removeNetworkProtocolHeader(Packet *packet, const Protocol& protocol);

};

#endif    // __INET_L3TOOLS_H
