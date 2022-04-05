// src/inet/networklayer/ipv4/Ipv4ServiceHeaderSerializer.h

#ifndef __INET_IPV4SERVICEHEADERSERIALIZER_H
#define __INET_IPV4SERVICEHEADERSERIALIZER_H

#include "inet/common/packet/serializer/FieldsChunkSerializer.h"
#include "inet/networklayer/ipv4/Ipv4ServiceHeader_m.h"

namespace inet {
/**
 * Converts between Ipv4ServiceHeader and binary (network byte order) Ipv4 Service header.
 */
class INET_API Ipv4ServiceHeaderSerializer : public FieldsChunkSerializer
{
  protected:
    virtual void serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const override;
    virtual const Ptr<Chunk> deserialize(MemoryInputStream& stream) const override;
  public:
    Ipv4ServiceHeaderSerializer() : FieldsChunkSerializer() {}
};

} // namespace inet

#endif // ifndef __INET_IPV4SERVICEHEADERSERIALIZER_H

