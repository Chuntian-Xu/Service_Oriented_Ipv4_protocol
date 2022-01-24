// inet/tutorials/configurator/SoI_Architecture_v2.ned
 
package inet.tutorials.configurator;

import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

import inet.node.base.MyApplicationLayerNodeBase_Udp_Ipv4;
import inet.node.inet.MyStandardHost_Udp_Ipv4;
import inet.node.inet.MyRouter_Udp_Ipv4_Wireless_MobileStation;
import inet.node.inet.MyRouter_Udp_Ipv4_Wireless_ShoreStation;
import inet.node.inet.MyRouter_Udp_Ipv4;
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;

import inet.linklayer.ethernet.EtherHub;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10M;

network SoI_Architecture_v2
{
    @display("bgb=1497.6058,682.8462");
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=558.6923,86.9077");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=737.1635,86.9077");
        }
        radioMedium: UnitDiskRadioMedium {
            @display("p=928.05005,83.80385");
        }

        ServiceClient1: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=108.63462,232.78848");
        }
        ServiceClient2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=108.63462,395.74042");
        }
        ServiceClient3: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=108.63462,566.45197");
        }

        Server1: MyStandardHost_Udp_Ipv4 {
            @display("i=device/server;p=1412.2501,234.3404");
        }
        Server2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/server;p=1412.2501,394.18848");
        }
        Server3: MyStandardHost_Udp_Ipv4 {
            @display("i=device/server;p=1412.2501,564.9");
        }

        hub0: EtherHub {
            @display("p=285.55386,395.74042");
        }
        hub1: EtherHub {
            @display("p=1216.7078,392.63657");
        }

        MobileStation: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("p=426.77887,395.74042");
        }
        ShoreStation: MyRouter_Udp_Ipv4_Wireless_ShoreStation {
            @display("p=676.6385,394.18848");
        }
        router: MyRouter_Udp_Ipv4 {
            @display("p=1039.7886,392.63657");
        }
    connections:
        MobileStation.ethg++ <--> Eth100M <--> hub0.ethg++;
        ServiceClient1.ethg++ <--> Eth100M <--> hub0.ethg++;
        ServiceClient2.ethg++ <--> Eth100M <--> hub0.ethg++;
        ServiceClient3.ethg++ <--> Eth100M <--> hub0.ethg++;

        ShoreStation.ethg++ <--> Eth100M <--> router.ethg++;
        router.ethg++ <--> Eth100M <--> hub1.ethg++;
        Server1.ethg++ <--> Eth100M <--> hub1.ethg++;
        Server2.ethg++ <--> Eth100M <--> hub1.ethg++;
        Server3.ethg++ <--> Eth100M <--> hub1.ethg++;
}
