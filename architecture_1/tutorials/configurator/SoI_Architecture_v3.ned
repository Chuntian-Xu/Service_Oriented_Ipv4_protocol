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
    @display("bgb=1617.9049,828.4366,white");
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
            @display("i=device/laptop;p=151.54327,220.8202;is=l");
        }
        ServiceClient2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=151.54327,399.78558;is=l");
        }
        MobileStation_1: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("p=521.0202,313.18942;is=l");
        }

        ServiceClient3: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=151.54327,587.4106;is=l");
        }
        ServiceClient4: MyStandardHost_Udp_Ipv4 {
            @display("i=device/laptop;p=151.54327,753.3866;is=l");
        }
        MobileStation_2: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("p=521.0202,653.80096;is=l");
        }

        Server1: MyStandardHost_Udp_Ipv4 {
            @display("i=device/server;p=1499.5568,376.6933;is=l");
        }
        Server2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/server;p=1499.5568,575.86444;is=l");
        }
        hub_1: EtherHub {
            @display("p=324.7356,314.6327;is=l");
        }
        hub_2: EtherHub {
            @display("p=324.7356,653.80096;is=l");
        }

        ShoreStation: MyRouter_Udp_Ipv4_Wireless_ShoreStation {
            @display("p=736.0673,487.825;is=l");
        }

        router: MyRouter_Udp_Ipv4 {
            @display("p=966.9904,489.26828;is=l");
        }
        internet: InternetCloud {

            @display("p=1173.3779,490.71155;is=l");
        }
        router1: MyRouter_Udp_Ipv4 {
            @display("p=1346.5702,418.5481;is=l");
        }
        router2: MyRouter_Udp_Ipv4 {
            @display("p=1346.5702,535.4529;is=l");
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
