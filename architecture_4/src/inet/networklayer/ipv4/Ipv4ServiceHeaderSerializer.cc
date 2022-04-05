// src/inet/networklayer/ipv4/Ipv4ServiceHeaderSerializer.cc

#include "inet/common/Endian.h"
#include "inet/common/packet/serializer/ChunkSerializerRegistry.h"
#include "Ipv4ServiceHeaderSerializer.h"
#include "headers/service_id.h"

namespace inet {

Register_Serializer(Ipv4ServiceHeader, Ipv4ServiceHeaderSerializer);

void Ipv4ServiceHeaderSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const {
//    EV_INFO<<"!!! --> Ipv4ServiceHeaderSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const\n";
    auto startPosition = stream.getLength();
    struct service_id id;
    const auto& ipv4ServiceHeader = staticPtrCast<const Ipv4ServiceHeader>(chunk);

    id.id_p = ipv4ServiceHeader->getProtocolId();

    id.service_id.s_addr = htonl(ipv4ServiceHeader->getServiceId().getInt());
    stream.writeBytes((uint8_t *)&id, IPv4Service_MIN_HEADER_LENGTH);
}

const Ptr<Chunk> Ipv4ServiceHeaderSerializer::deserialize(MemoryInputStream& stream) const {
    auto position = stream.getPosition();
    B bufsize = stream.getRemainingLength();
    uint8_t buffer[B(IPv4Service_MIN_HEADER_LENGTH).get()];
    stream.readBytes(buffer, IPv4Service_MIN_HEADER_LENGTH);
    auto ipv4ServiceHeader = makeShared<Ipv4ServiceHeader>();
    const struct service_id& id = *static_cast<const struct service_id *>((void *)&buffer);

    ipv4ServiceHeader->setServiceId(ServiceId(uint16(ntohl(id.service_id.s_addr))));
    ipv4ServiceHeader->setProtocolId((IpProtocolId)id.id_p);

    return ipv4ServiceHeader;
}
} // namespace inet

