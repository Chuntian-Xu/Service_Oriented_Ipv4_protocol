// src/inet/networklayer/ipv4/Ipv4HeaderService.msg

import inet.common.INETDefs;
import inet.common.TlvOptions;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.IpProtocolId;
import inet.networklayer.contract.NetworkHeaderBase;
import inet.networklayer.contract.ipv4.Ipv4Address;
import inet.transportlayer.common.CrcMode;

import inet.networklayer.contract.serviceid.ServiceId;  // new added

cplusplus {{
#include "inet/common/ProtocolGroup.h"
}}

namespace inet;

cplusplus {{
// default IPv4ServiceID header length: 4 bytes
const B IPv4ServiceID_MIN_HEADER_LENGTH = B(4);

// maximum IPv4ServiceID header length (base+options): 6+4 = 10 bytes
const B IPv4ServiceID_MAX_HEADER_LENGTH = B(10);

}}

// Represents an IPv4ServiceID datagram.
//
// Uses the following cPacket fields:
//    - getByteLength() / setByteLength() to represent total datagram length
// Additional length fields defined in this class are in bytes.
//
class Ipv4HeaderService extends NetworkHeaderBase 
{
    chunkLength = IPv4ServiceID_MIN_HEADER_LENGTH;
    B headerLength = IPv4ServiceID_MIN_HEADER_LENGTH;   
    ServiceId srcServiceId;
    ServiceId destServiceId;
}

cplusplus(Ipv4HeaderService) {{
  public:
    // implements NetworkHeaderBase:
    virtual L3Address getSourceAddress() const override { return L3Address(getSrcServiceId()); }
    virtual void setSourceAddress(const L3Address& address) override { setSrcServiceId(address.toServiceId()); }
    virtual L3Address getDestinationAddress() const override { return L3Address(getDestServiceId()); }
    virtual void setDestinationAddress(const L3Address& address) override { setDestServiceId(address.toServiceId()); }
    virtual const Protocol *getProtocol() const override {}
    virtual void setProtocol(const Protocol *protocol) override {}
    virtual bool isFragment() const override {}
}}
