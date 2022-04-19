// src/inet/networklayer/ipv4/Ipv4Cid.msg

import inet.common.INETDefs;
import inet.networklayer.contract.ipv4.Ipv4Address;
import inet.networklayer.contract.clientid.ClientId;

cplusplus {{
#include "inet/networklayer/ipv4/Ipv4Cid.h"
}}

namespace inet;

class Ipv4Cid extends cNamedObject{
    @existingClass;
    @descriptor(readonly);
	Ipv4Address ipaddr @byValue;
	ClientId cid @byValue;
	int port;
	string interfaceName;
}

