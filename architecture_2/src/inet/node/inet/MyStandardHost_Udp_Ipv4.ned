// src/inet/node/inet/MyStandardHost_Udp_Ipv4.ned

package inet.node.inet;

import inet.node.base.MyApplicationLayerNodeBase_Udp_Ipv4;

module MyStandardHost_Udp_Ipv4 extends MyApplicationLayerNodeBase_Udp_Ipv4 {
    parameters:
        @display("i=device/pc2");
        @figure[submodules];
        ipv4.arp.proxyArpInterfaces = default(""); // proxy arp is disabled on hosts by default
}
