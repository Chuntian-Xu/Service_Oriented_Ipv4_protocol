// src/inet/networklayer/ipv4/Ipv4ServiceHeaderSerializer.cc

#include "inet/common/Endian.h"
#include "inet/common/packet/serializer/ChunkSerializerRegistry.h"
#include "inet/networklayer/ipv4/headers/ip_service.h"
#include "Ipv4ServiceHeaderSerializer.h"

namespace inet {

Register_Serializer(Ipv4ServiceHeader, Ipv4ServiceHeaderSerializer);

void Ipv4ServiceHeaderSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const {
//    EV_INFO<<"!!! --> Ipv4ServiceHeaderSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const\n";
    auto startPosition = stream.getLength();
    struct ip_service iphdr;
    const auto& ipv4ServiceHeader = staticPtrCast<const Ipv4ServiceHeader>(chunk);
    ASSERT((ipv4ServiceHeader->getFragmentOffset() & 7) == 0);

    uint16_t ip_off = (ipv4ServiceHeader->getFragmentOffset() / 8) & IP_OFFMASK;
    if (ipv4ServiceHeader->getReservedBit()) ip_off |= IP_RF;
    if (ipv4ServiceHeader->getMoreFragments()) ip_off |= IP_MF;
    if (ipv4ServiceHeader->getDontFragment()) ip_off |= IP_DF;
    iphdr.ip_off = htons(ip_off);

    iphdr.ip_p = ipv4ServiceHeader->getProtocolId();
    iphdr.ip_service_src.s_addr = htonl(ipv4ServiceHeader->getSrcServiceId().getInt());
    iphdr.ip_service_dst.s_addr = htonl(ipv4ServiceHeader->getDestServiceId().getInt());
    stream.writeBytes((uint8_t *)&iphdr, IPv4ServiceID_MIN_HEADER_LENGTH);
}

const Ptr<Chunk> Ipv4ServiceHeaderSerializer::deserialize(MemoryInputStream& stream) const
{
    auto position = stream.getPosition();
    B bufsize = stream.getRemainingLength();
    uint8_t buffer[B(IPv4ServiceID_MIN_HEADER_LENGTH).get()];
    stream.readBytes(buffer, IPv4ServiceID_MIN_HEADER_LENGTH);
    auto ipv4ServiceHeader = makeShared<Ipv4ServiceHeader>();
    const struct ip_service& iphdr = *static_cast<const struct ip_service *>((void *)&buffer);

    ipv4ServiceHeader->setSrcServiceId(ServiceId(ntohl(iphdr.ip_service_src.s_addr)));
    ipv4ServiceHeader->setDestServiceId(ServiceId(ntohl(iphdr.ip_service_dst.s_addr)));
    ipv4ServiceHeader->setProtocolId((IpProtocolId)iphdr.ip_p);
    uint16_t ip_off = ntohs(iphdr.ip_off);
    ipv4ServiceHeader->setReservedBit((ip_off & IP_RF) != 0);
    ipv4ServiceHeader->setMoreFragments((ip_off & IP_MF) != 0);
    ipv4ServiceHeader->setDontFragment((ip_off & IP_DF) != 0);
    ipv4ServiceHeader->setFragmentOffset((ntohs(iphdr.ip_off) & IP_OFFMASK) * 8);

    return ipv4ServiceHeader;
}
} // namespace inet

