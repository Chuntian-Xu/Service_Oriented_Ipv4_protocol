// src/inet/networklayer/ipv4/headers/service_client_id.h

#ifndef __INET_SERVICE_CLIENT_ID_H
#define __INET_SERVICE_CLIENT_ID_H

namespace inet {

struct service_client_id
{
    u_char id_p;     // protocol
    struct  in_addr client_id;
    struct  in_addr service_id;
};
} // namespace inet

#endif // ifndef __INET_SERVICE_CLIENT_ID_H
