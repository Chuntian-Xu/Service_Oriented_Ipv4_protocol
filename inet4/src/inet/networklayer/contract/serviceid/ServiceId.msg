// src/inet/networklayer/contract/serviceid/ServiceId.msg 

import inet.common.INETDefs;

cplusplus {{
#include "inet/networklayer/contract/serviceid/ServiceId.h"
}}

namespace inet;

class ServiceId {
    @existingClass;
    @opaque;
    @toString(.str());
    @fromString(ServiceId($));
}

