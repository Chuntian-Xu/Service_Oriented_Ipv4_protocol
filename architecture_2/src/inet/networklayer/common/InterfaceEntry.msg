// inet/networklayer/common/InterfaceEntry.msg

import inet.common.INETDefs;
import inet.linklayer.common.MacAddress;
import inet.networklayer.contract.ipv4.Ipv4Address;

cplusplus {{
#include "inet/networklayer/common/InterfaceEntry.h"
}}

namespace inet;

class InterfaceEntry extends cModule
{
    @existingClass;
    @descriptor(readonly);
    bool up @getter("isUp");
    bool broadcast @getter("isBroadcast");
    bool multicast @getter("isMulticast");
    bool pointToPoint @getter("isPointToPoint");
    bool loopback @getter("isLoopback");
    int mtu;
    MacAddress macAddress;
    Ipv4Address ipv4Address @byValue;
    Ipv4Address ipv4Netmask @byValue;
}

