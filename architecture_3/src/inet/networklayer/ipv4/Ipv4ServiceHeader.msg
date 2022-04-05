// src/inet/networklayer/ipv4/Ipv4ServiceHeader.msg

import inet.common.INETDefs;
import inet.common.TlvOptions;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.IpProtocolId;

import inet.networklayer.contract.NetworkServiceHeaderBase; 
import inet.networklayer.contract.clientid.ClientId;  
import inet.networklayer.contract.serviceid.ServiceId; 

cplusplus {{
#include "inet/common/ProtocolGroup.h"
}}

namespace inet;

cplusplus {{
// default IPv4Service header length: 3 bytes
const B IPv4Service_MIN_HEADER_LENGTH = B(3);
// maximum IPv4Service header length (base+options): 3+1 = 4 bytes
const B IPv4Service_MAX_HEADER_LENGTH = B(4);
}}

// Represents an IPv4Service datagram.

class Ipv4ServiceHeader extends NetworkServiceHeaderBase 
{
    chunkLength = IPv4Service_MIN_HEADER_LENGTH;
    IpProtocolId protocolId = IP_PROT_NONE; // @byte(1)
    ClientId clientId;  // @byte(1)
    ServiceId serviceId;  // @byte(2)
}

cplusplus(Ipv4ServiceHeader) {{
  public:
    // implements NetworkServiceHeaderBase:
    virtual L3Address getSourceAddress() const override { return L3Address(getClientId()); }
    virtual void setSourceAddress(const L3Address& address) override { setClientId(address.toClientId()); }
    virtual L3Address getDestinationAddress() const override { return L3Address(getServiceId()); }
    virtual void setDestinationAddress(const L3Address& address) override { setServiceId(address.toServiceId()); }
    
    virtual const Protocol *getProtocol() const override {
        return ProtocolGroup::ipprotocol.findProtocol(getProtocolId()); 
	}
    virtual void setProtocol(const Protocol *protocol) override {
        setProtocolId(static_cast<IpProtocolId>(ProtocolGroup::ipprotocol.getProtocolNumber(protocol)));
	}
}}
