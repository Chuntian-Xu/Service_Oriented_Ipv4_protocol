// src/inet/networklayer/contract/ICidshore.cc

#include "inet/networklayer/contract/ICidshore.h"

namespace inet {

const char *ICidshore::sourceTypeName(SourceType sourceType) {
    switch (sourceType) {
        case MANUAL:
            return "MANUALCID";
        default:
            return "???";
    }
}

} // namespace inet

