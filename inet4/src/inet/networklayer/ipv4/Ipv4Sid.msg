// src/inet/networklayer/ipv4/Ipv4Sid.msg 

import inet.common.INETDefs;
import inet.networklayer.contract.ipv4.Ipv4Address;
import inet.networklayer.contract.serviceid.ServiceId;

cplusplus {{
#include "inet/networklayer/ipv4/Ipv4Sid.h"
}}

namespace inet;

class Ipv4Sid extends cNamedObject{
    @existingClass;
    @descriptor(readonly);
	Ipv4Address ipaddr @byValue;
	ServiceId sid @byValue;
	string interfaceName;
}

