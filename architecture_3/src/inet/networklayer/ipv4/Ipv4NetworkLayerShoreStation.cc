// src/inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.cc

#include "inet/common/StringFormat.h"
#include "inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.h"

namespace inet {

Define_Module(Ipv4NetworkLayerShoreStation);

void Ipv4NetworkLayerShoreStation::refreshDisplay() const
{
    updateDisplayString();
}

void Ipv4NetworkLayerShoreStation::updateDisplayString() const
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

