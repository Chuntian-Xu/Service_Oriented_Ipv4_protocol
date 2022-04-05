// src/inet/networklayer/ipv4/Ipv4Cid.cc

#include <sstream>
#include <stdio.h>

#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/ipv4/IIpv4RoutingTable.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/ipv4/Ipv4Cid.h"
#include "inet/networklayer/ipv4/Ipv4CidTable.h"

namespace inet {

Register_Class(Ipv4Cid);

Ipv4Cid::~Ipv4Cid(){}

const char* inet::Ipv4Cid::getSourceTypeAbbreviation() const {
    switch (sourceType) {
        case IFACENETMASK:
            return "C";
        case MANUAL:
            return (getIpaddr().isUnspecified() ? "S*": "S");
        case ROUTER_ADVERTISEMENT:
            return "ra";
        case RIP:
            return "R";
        case OSPF:
            return "O";
        case BGP:
            return "B";
        case EIGRP:
            return getAdminDist() < Ipv4Route::dEIGRPExternal ? "D" : "D EX";
        case LISP:
            return "l";
        case BABEL:
            return "ba";
        case ODR:
            return "o";
        default:
            return "???";
    }
}

std::string Ipv4Cid::str() const {
    std::stringstream out;
    out << getSourceTypeAbbreviation();
    if (getIpaddr().isUnspecified()) out << "0.0.0.0";
    else out << " IpAddr: " << getIpaddr();
    out << " Port: " << port; // new added
    if (getCid().isUnspecified()) out << "0";
    else out << " ClientId: " << getCid();
    return out.str();
}

std::string Ipv4Cid::detailedInfo() const {
    return std::string();
}

const char *Ipv4Cid::getInterfaceName() const {
    return interfacePtr ? interfacePtr->getInterfaceName() : "";
}

void Ipv4Cid::changed(int fieldCode) {
    if (rt) rt->ipv4CidChanged(this, fieldCode);
}

ICidTable *Ipv4Cid::getCidTableAsGeneric() const {
    return getCidTable();
}

} // namespace inet

