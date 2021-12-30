// src/inet/networklayer/ipv4/Ipv4ServiceHeader.msg

import inet.common.INETDefs;
import inet.common.TlvOptions;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.IpProtocolId;
import inet.networklayer.contract.NetworkHeaderBase;
import inet.transportlayer.common.CrcMode;

import inet.networklayer.contract.serviceid.ServiceId;  // new added

cplusplus {{
#include "inet/common/ProtocolGroup.h"
}}

namespace inet;

cplusplus {{
// default IPv4ServiceID header length: 7 bytes
const B IPv4ServiceID_MIN_HEADER_LENGTH = B(7);
// maximum IPv4ServiceID header length (base+options): 7+3 = 10 bytes
const B IPv4ServiceID_MAX_HEADER_LENGTH = B(10);
}}

// Represents an IPv4ServiceID datagram.

class Ipv4ServiceHeader extends NetworkHeaderBase 
{
    chunkLength = IPv4ServiceID_MIN_HEADER_LENGTH;
    
    bool reservedBit = false;    // @bit(1)
    bool moreFragments;    // @bit(1)
    bool dontFragment;    // @bit(1)
    uint16_t fragmentOffset; // @bit(13),must be multiple of 8
    
    IpProtocolId protocolId = IP_PROT_NONE; // @byte(1)
    
    ServiceId srcServiceId;  // @byte(2)
    ServiceId destServiceId;  // @byte(2)
}

cplusplus(Ipv4ServiceHeader) {{
  public:
    // implements NetworkHeaderBase:
    virtual L3Address getSourceAddress() const override { return L3Address(getSrcServiceId()); }
    virtual void setSourceAddress(const L3Address& address) override { setSrcServiceId(address.toServiceId()); }
    virtual L3Address getDestinationAddress() const override { return L3Address(getDestServiceId()); }
    virtual void setDestinationAddress(const L3Address& address) override { setDestServiceId(address.toServiceId()); }
    virtual const Protocol *getProtocol() const override {
        return ProtocolGroup::ipprotocol.findProtocol(getProtocolId()); 
	}
    virtual void setProtocol(const Protocol *protocol) override {
        setProtocolId(static_cast<IpProtocolId>(ProtocolGroup::ipprotocol.getProtocolNumber(protocol)));
	}
	virtual bool isFragment() const override { 
        return getMoreFragments() || (getFragmentOffset() != 0); 
	}
}}
