// src/inet/networklayer/contract/ISid.cc

#include "inet/networklayer/contract/ISid.h"

namespace inet {

const char *ISid::sourceTypeName(SourceType sourceType) {
    switch (sourceType) {
        case MANUAL:
            return "MANUALSID";
        default:
            return "???";
    }
}

} // namespace inet

