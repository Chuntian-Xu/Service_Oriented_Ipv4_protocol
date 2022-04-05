// src/inet/networklayer/contract/NetworkServiceHeaderBase.msg 

import inet.common.INETDefs;
import inet.common.ProtocolTag;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

cplusplus {{

namespace inet {

class INET_API NetworkServiceHeaderBase : public FieldsChunk
{
  protected:
    virtual NetworkServiceHeaderBase *dup() const override { throw omnetpp::cRuntimeError("You forgot to manually add a dup() function to class %s", getClassName()); }
  public:
    virtual L3Address getDestinationAddress() const = 0;
    virtual void setDestinationAddress(const L3Address& address) = 0;
    
    virtual const Protocol *getProtocol() const = 0;
    virtual void setProtocol(const Protocol *protocol) = 0;
};

} // namespace inet

}}

namespace inet;

class NetworkServiceHeaderBase extends FieldsChunk
{
    @existingClass;

    L3Address destinationAddress @byValue;
    const Protocol *protocol;
};

