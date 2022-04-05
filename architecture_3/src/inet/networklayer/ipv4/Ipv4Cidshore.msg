// src/inet/networklayer/ipv4/Ipv4Cidshore.msg

import inet.common.INETDefs;
import inet.networklayer.contract.ipv4.Ipv4Address;
import inet.networklayer.contract.clientid.ClientId;
import inet.linklayer.common.MacAddress;

cplusplus {{
#include "inet/networklayer/ipv4/Ipv4Cidshore.h"
}}

namespace inet;

class Ipv4Cidshore extends cNamedObject{
    @existingClass;
    @descriptor(readonly);
	Ipv4Address ipaddr @byValue;
	ClientId cid @byValue;
	MacAddress MMSI @byValue; // new added
	int port;
	string interfaceName;
}

