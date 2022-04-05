// inet/tutorials/configurator/SoI_Architecture_v3.ned

package inet.tutorials.configurator;

import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

import inet.node.base.MyApplicationLayerNodeBase_Udp_Ipv4;
import inet.node.inet.MyStandardHost_Udp_Ipv4;
import inet.node.inet.MyRouter_Udp_Ipv4_Wireless_MobileStation;

import inet.linklayer.ethernet.EtherHub;
import inet.node.ethernet.Eth100M;
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;

network SoI_Architecture_v3_m2m_2m
{
    @display("bgb=1415.8472,600.4");
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
            @display("i=device/laptop;p=129.89424,223.70674");
        }
        ServiceClient2: MyStandardHost_Udp_Ipv4 {
            @display("i=device/pc2;p=129.89424,411.33176");
        }
        hub_0: EtherHub {
            @display("p=355.04425,323.29233");
        }
        MobileStation_0: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("i=device/accesspoint;p=578.751,323.29233");
        }

        MobileStation_1: MyRouter_Udp_Ipv4_Wireless_MobileStation {
            @display("i=device/accesspoint;p=851.5289,323.29233");
        }
        hub_1: EtherHub {
            @display("p=1062.2462,324.7356");
        }
        ServiceClient3: MyStandardHost_Udp_Ipv4 {
            @display("p=1283.0664,220.8202;i=device/laptop");
        }
        ServiceClient4: MyStandardHost_Udp_Ipv4 {
            @display("p=1283.0664,419.99136;i=device/pc3");
        }

    connections:
        MobileStation_0.ethg++ <--> Eth100M <--> hub_0.ethg++;
        ServiceClient1.ethg++ <--> Eth100M <--> hub_0.ethg++;
        ServiceClient2.ethg++ <--> Eth100M <--> hub_0.ethg++;

        MobileStation_1.ethg++ <--> Eth100M <--> hub_1.ethg++;
        ServiceClient3.ethg++ <--> Eth100M <--> hub_1.ethg++;
        ServiceClient4.ethg++ <--> Eth100M <--> hub_1.ethg++;
}
