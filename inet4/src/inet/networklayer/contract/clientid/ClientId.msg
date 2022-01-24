// src/inet/networklayer/contract/clientid/ClientId.msg
 
import inet.common.INETDefs;

cplusplus {{
#include "inet/networklayer/contract/clientid/ClientId.h"
}}

namespace inet;

class ClientId {
    @existingClass;
    @opaque;
    @toString(.str());
    @fromString(ClientId($));
}

