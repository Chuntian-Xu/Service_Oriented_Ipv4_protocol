// src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.cc 

#include "inet/common/StringFormat.h"
#include "inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.h"

namespace inet {

Define_Module(Ipv4NetworkLayerMobileStation);

void Ipv4NetworkLayerMobileStation::refreshDisplay() const
{
    updateDisplayString();
}

void Ipv4NetworkLayerMobileStation::updateDisplayString() const
{
    auto text = StringFormat::formatString(par("displayStringTextFormat"), [&] (char directive) {
        static std::string result;
        switch (directive) {
            case 'i':
                result = getSubmodule("ip")->getDisplayString().getTagArg("t", 0);
                break;
            default:
                throw cRuntimeError("Unknown directive: %c", directive);
        }
        return result.c_str();
    });
    getDisplayString().setTagArg("t", 0, text);
}

} // namespace inet

