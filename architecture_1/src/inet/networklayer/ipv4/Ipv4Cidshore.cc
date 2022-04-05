// src/inet/networklayer/ipv4/Ipv4Cidshore.cc

#include <sstream>
#include <stdio.h>

#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/ipv4/IIpv4RoutingTable.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/ipv4/Ipv4Cidshore.h"
#include "inet/networklayer/ipv4/Ipv4CidshoreTable.h"

namespace inet {

Register_Class(Ipv4Cidshore);

Ipv4Cidshore::~Ipv4Cidshore(){}

const char* inet::Ipv4Cidshore::getSourceTypeAbbreviation() const {
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

std::string Ipv4Cidshore::str() const {
    std::stringstream out;
    out << getSourceTypeAbbreviation();
    if (getIpaddr().isUnspecified()) out << "0.0.0.0";
    else out << " IpAddr: " << getIpaddr();
    out << " Port: " << port;
    if (getCid().isUnspecified()) out << "0";
    else out << " ClientId: " << getCid();
    if (getMMSI().isUnspecified()) out << "0:0:0:0:0:0"; // new added
    else out << " MMSI: " << getMMSI(); // new added
    return out.str();
}

std::string Ipv4Cidshore::detailedInfo() const {
    return std::string();
}

const char *Ipv4Cidshore::getInterfaceName() const {
    return interfacePtr ? interfacePtr->getInterfaceName() : "";
}

void Ipv4Cidshore::changed(int fieldCode) {
    if (rt) rt->ipv4CidshoreChanged(this, fieldCode);
}

ICidshoreTable *Ipv4Cidshore::getCidshoreTableAsGeneric() const {
    return getCidshoreTable();
}

} // namespace inet

