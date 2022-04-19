// inet/tutorials/configurator/SoI_Architecture_v3.ned

package inet.tutorials.configurator;

import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

import inet.node.base.MyApplicationLayerNodeBase_Udp_Ipv4;
import inet.node.inet.MyStandardHost_Udp_Ipv4;
import inet.node.inet.MyRouter_Udp_Ipv4_Wireless_MobileStation;
import inet.node.inet.MyRouter_Udp_Ipv4_Wireless_ShoreStation;
import inet.node.inet.MyRouter_Udp_Ipv4;

import inet.linklayer.ethernet.EtherHub;
import inet.node.ethernet.Eth100M;
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;
import inet.node.fiberlink.FiberLink;

import inet.node.internetcloud.InternetCloud;

network SoI_Architecture_v3
{
    @display("bgb=1555.4113,949.3825");
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
            @display("i=device/laptop;p=123.832504,245.78876");
        }
        ServiceClient2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=123.832504,433.41376");
        }
        hub_1: EtherHub {
            @display("p=302.07626,347.10626");
        }
        MobileStation_1: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("p=484.0725,345.23");
        }

        ServiceClient3: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=123.832504,622.91504");
        }
        ServiceClient4: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=123.832504,823.67377");
        }
        hub_2: EtherHub {
            @display("p=302.07626,707.34625");
        }
        MobileStation_2: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("p=484.0725,705.47003");
        }

        Server1: MyStandardHost_Udp_Ipv4 {
            @display("i=device/server;p=1482.2375,435.29");
        }
        Server2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/server;p=1482.2375,634.17255");
        }
        router1: MyRouter_Udp_Ipv4 {
            @display("p=1328.385,476.5675");
        }
        router2: MyRouter_Udp_Ipv4 {
            @display("p=1328.385,592.895");
        }
        internet: InternetCloud {

            @display("p=1155.77,547.865");
        }
        router: MyRouter_Udp_Ipv4 {
            @display("p=949.3825,547.865");
        }
        ShoreStation: MyRouter_Udp_Ipv4_Wireless_ShoreStation {
            @display("p=718.60376,545.9888");
        }

    connections:
        MobileStation_1.ethg++ <--> Eth100M <--> hub_1.ethg++;
        ServiceClient1.ethg++ <--> Eth100M <--> hub_1.ethg++;
        ServiceClient2.ethg++ <--> Eth100M <--> hub_1.ethg++;

        MobileStation_2.ethg++ <--> Eth100M <--> hub_2.ethg++;
        ServiceClient3.ethg++ <--> Eth100M <--> hub_2.ethg++;
        ServiceClient4.ethg++ <--> Eth100M <--> hub_2.ethg++;

        ShoreStation.ethg++ <--> Eth100M <--> router.ethg++;
        router.ethg++ <--> FiberLink <--> internet.ethg++;
        router1.ethg++ <--> FiberLink <--> internet.ethg++;
        router2.ethg++ <--> FiberLink <--> internet.ethg++;
        Server1.ethg++ <--> FiberLink <--> router1.ethg++;
        Server2.ethg++ <--> FiberLink <--> router2.ethg++;
}
