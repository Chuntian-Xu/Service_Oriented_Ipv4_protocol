// src/inet/networklayer/contract/ICid.cc
 
#include "inet/networklayer/contract/ICid.h"

namespace inet {

const char *ICid::sourceTypeName(SourceType sourceType) {
    switch (sourceType) {
        case MANUAL:
            return "MANUALCID";
        default:
            return "???";
    }
}

} // namespace inet

