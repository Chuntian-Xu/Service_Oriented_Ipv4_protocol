// src/inet/networklayer/ipv4/headers/ip_service.h

#ifndef __INET_IP_SERVICE_H
#define __INET_IP_SERVICE_H

namespace inet {

#define IPVERSION    4

struct ip_service
{
    #define IP_RF         0x8000  // reserved fragment flag
    #define IP_DF         0x4000  // dont fragment flag
    #define IP_MF         0x2000  // more fragments flag
    #define IP_OFFMASK    0x1fff  // mask for fragmenting bits
    u_short ip_off;  // fragment offset field
    u_char ip_p;     // protocol
    struct  in_addr ip_service_src, ip_service_dst;    // source and dest address
};
} // namespace inet

#endif // ifndef __INET_IP_SERVICE_H
