// src/inet/networklayer/contract/ISid.h 

#ifndef __INET_ISID_H
#define __INET_ISID_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/common/L3Address.h"

namespace inet {

class ISidTable;

/**
 * C++ interface for accessing ServiceId table entries of various protocols (IPv4, IPv6, etc) in a uniform way.
 *
 * @see ISidTable
 */
class INET_API ISid {
  public:
    /** Specifies where the sid comes from */
    enum SourceType {
            MANUAL,    ///< manually added static route
            IFACENETMASK,    ///< comes from an interface's netmask
            ROUTER_ADVERTISEMENT,    ///< on-link prefix, from Router Advertisement
            OWN_ADV_PREFIX,    ///< on routers: on-link prefix that the router **itself** advertises on the link
            ICMP_REDIRECT,    ///< ICMP redirect message
            RIP,    ///< managed by the given routing protocol
            OSPF,    ///< managed by the given routing protocol
            BGP,    ///< managed by the given routing protocol
            ZEBRA,    ///< managed by the Quagga/Zebra based model
            MANET,    ///< managed by manet, search exact address
            MANET2,    ///< managed by manet, search approximate address
            DYMO,    ///< managed by DYMO routing
            AODV,    ///< managed by AODV routing
            EIGRP, LISP, BABEL, ODR, UNKNOWN, ISIS
        };
    /** Field codes for NB_ROUTE_CHANGED notifications */
    enum ChangeCodes {
        F_DESTINATION,
        F_PREFIX_LENGTH,
        F_NEXTHOP,
        F_IFACE,
        F_SOURCE,
        F_TYPE,
        F_ADMINDIST,
        F_METRIC,
        F_EXPIRYTIME,
        F_LAST
    };
    /** Cisco like administrative distances */
    enum RouteAdminDist {
        dDirectlyConnected = 0,
        dStatic = 1,
        dEIGRPSummary = 5,
        dBGPExternal = 20,
        dEIGRPInternal = 90,
        dIGRP = 100,
        dOSPF = 110,
        dISIS = 115,
        dRIP = 120,
        dEGP = 140,
        dODR = 160,
        dEIGRPExternal = 170,
        dBGPInternal = 200,
        dDHCPlearned = 254,
        dBABEL = 125,
        dLISP = 210,
        dUnknown = 255
    };

    virtual ~ISid() {}

    virtual ISidTable *getSidTableAsGeneric() const = 0;

    /** operation: set */
    virtual void setIpaddr(const L3Address& ipaddr) = 0;
    virtual void setSid(const L3Address& sid) = 0;
    virtual void setInterface(InterfaceEntry *ie) = 0;
    virtual void setSourceType(SourceType type) = 0;
    /** operation: get */
    virtual L3Address getIpaddrAsGeneric() const = 0;
    virtual ServiceId getSidAsGeneric() const = 0;
    virtual ServiceId getSid() const = 0;
    virtual InterfaceEntry *getInterface() const = 0;
    virtual SourceType getSourceType() const = 0;

    static const char *sourceTypeName(SourceType sourceType);
};

inline std::ostream& operator<<(std::ostream& out, const ISid *sid) {
    out << "ipaddr = " << sid->getIpaddrAsGeneric();
    out << "service id = " << sid->getSidAsGeneric();
    if (auto ie = sid->getInterface()) out << ", interface = " << ie->getInterfaceName();
    return out;
};

} // namespace inet

#endif // ifndef __INET_ISID_H

