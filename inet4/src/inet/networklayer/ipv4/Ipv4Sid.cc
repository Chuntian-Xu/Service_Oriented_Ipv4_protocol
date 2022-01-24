// src/inet/networklayer/ipv4/Ipv4Sid.cc 

#include <sstream>
#include <stdio.h>

#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/ipv4/IIpv4RoutingTable.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/ipv4/Ipv4Sid.h"
#include "inet/networklayer/ipv4/Ipv4SidTable.h"

namespace inet {

Register_Class(Ipv4Sid);

Ipv4Sid::~Ipv4Sid(){}

const char* inet::Ipv4Sid::getSourceTypeAbbreviation() const {
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

std::string Ipv4Sid::str() const {
    std::stringstream out;
    out << getSourceTypeAbbreviation();
    if (getIpaddr().isUnspecified()) out << "0.0.0.0";
    else out << " " << getIpaddr();
    out << "  ";
    if (getSid().isUnspecified()) out << "0.0";
    else out << " " << getSid();
    return out.str();
}

std::string Ipv4Sid::detailedInfo() const {
    return std::string();
}

const char *Ipv4Sid::getInterfaceName() const {
    return interfacePtr ? interfacePtr->getInterfaceName() : "";
}

void Ipv4Sid::changed(int fieldCode) {
    if (rt) rt->ipv4SidChanged(this, fieldCode);
}

ISidTable *Ipv4Sid::getSidTableAsGeneric() const {
    return getSidTable();
}

} // namespace inet

