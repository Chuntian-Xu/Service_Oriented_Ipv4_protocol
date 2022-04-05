// inet/tutorials/configurator/SoI_Architecture_v3_3.ned

package inet.tutorials.configurator;

import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

import inet.node.base.MyApplicationLayerNodeBase_Udp_Ipv4;
import inet.node.inet.MyStandardHost_Udp_Ipv4;
import inet.node.inet.MyRouter_Udp_Ipv4_Wireless_MobileStation;

import inet.linklayer.ethernet.EtherHub;
import inet.node.ethernet.Eth100M;
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;

network SoI_Architecture_v3_m2m_3m
{
    @display("bgb=1431.7231,865.96155,white");
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
            @display("i=device/laptop;p=102.47212,236.69617;is=l");
        }
        ServiceClient2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/pc2;p=102.47212,424.32117;is=l");
        }
        hub_0: EtherHub {
            @display("p=327.62213,336.28174");
        }
        MobileStation_0: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("i=device/accesspoint;p=551.32886,336.28174;is=l");
        }

        MobileStation_1: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("i=device/accesspoint;p=824.10675,336.28174;is=l");
        }
        hub_1: EtherHub {
            @display("p=1034.8241,337.725");
        }
        ServiceClient3: MyStandardHost_Udp_Ipv4 {
            @display("p=1255.6443,233.80963;i=device/laptop;is=l");
        }
        ServiceClient4: MyStandardHost_Udp_Ipv4 {
            @display("p=1255.6443,432.98077;i=device/pc3;is=l");
        }

        MobileStation_2: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("i=device/accesspoint;p=679.77985,532.56635;is=l");
        }
        hub_2: EtherHub {
            @display("p=679.77985,672.5635");
        }
        ServiceClient5: MyStandardHost_Udp_Ipv4 {
            @display("p=486.38174,785.1385;i=device/laptop;is=l");
        }
        ServiceClient6: MyStandardHost_Udp_Ipv4 {
            @display("p=874.6212,785.1385;i=device/pc3;is=l");
        }

    connections:
        MobileStation_0.ethg++ <--> Eth100M <--> hub_0.ethg++;
        ServiceClient1.ethg++ <--> Eth100M <--> hub_0.ethg++;
        ServiceClient2.ethg++ <--> Eth100M <--> hub_0.ethg++;

        MobileStation_1.ethg++ <--> Eth100M <--> hub_1.ethg++;
        ServiceClient3.ethg++ <--> Eth100M <--> hub_1.ethg++;
        ServiceClient4.ethg++ <--> Eth100M <--> hub_1.ethg++;

        MobileStation_2.ethg++ <--> Eth100M <--> hub_2.ethg++;
        ServiceClient5.ethg++ <--> Eth100M <--> hub_2.ethg++;
        ServiceClient6.ethg++ <--> Eth100M <--> hub_2.ethg++;
}
