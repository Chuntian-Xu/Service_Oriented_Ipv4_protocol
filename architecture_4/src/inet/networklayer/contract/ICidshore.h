// src/inet/networklayer/contract/ICidshore.h

#ifndef __INET_ICIDSHORE_H
#define __INET_ICIDSHORE_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/linklayer/common/MacAddress.h" // new added

namespace inet {

class ICidshoreTable;

/**
 * C++ interface for accessing ClientId_shore table entries of various protocols (IPv4, IPv6, etc) in a uniform way.
 *
 * @see ICidshoreTable
 */
class INET_API ICidshore {
  public:
    /** Specifies where the ICidshore comes from */
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
        F_LAST,
        F_PORT,  // new added
        F_MMSI  // new added
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

    virtual ~ICidshore() {}

    virtual ICidshoreTable *getCidshoreTableAsGeneric() const = 0;

    /** operation: set */
    virtual void setIpaddr(const L3Address& ipaddr) = 0;
    virtual void setPort(int port) = 0;
    virtual void setMMSI(MacAddress MMSI) = 0; // new added

    virtual void setInterface(InterfaceEntry *ie) = 0;
    virtual void setSourceType(SourceType type) = 0;

    /** operation: get */
    virtual L3Address getIpaddrAsGeneric() const = 0;
    virtual int getPort() const = 0;
    virtual MacAddress getMMSI() const = 0; // new added

    virtual InterfaceEntry *getInterface() const = 0;
    virtual SourceType getSourceType() const = 0;

    static const char *sourceTypeName(SourceType sourceType);
};

inline std::ostream& operator<<(std::ostream& out, const ICidshore *cidshore) {
    out << "ip addr = " << cidshore->getIpaddrAsGeneric();
    out << "port = " << cidshore->getPort();
    out << "MMSI = " << cidshore->getMMSI();
    if (auto ie = cidshore->getInterface()) out << ", interface = " << ie->getInterfaceName();
    return out;
};

} // namespace inet

#endif // ifndef __INET_ICIDSHORE_H

