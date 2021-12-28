// /inet/tutorials/configurator/SoI_Architecture.ned

package inet.tutorials.configurator;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;

import inet.node.inet.MyStandardHost_Udp_Ipv4;
import inet.node.inet.MyRouter_Udp_Ipv4_Wireless_Station;
import inet.node.ethernet.EtherSwitch;

import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10M;

network SoI_Architecture {
    @display("bgb=1610.5125,874.96497");
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=129.12,98.8575");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=297.96924,97.77116");
        }
		radioMedium: UnitDiskRadioMedium {
            @display("p=452.92004,94.64001");
        }

        ServiceClient0: MyStandardHost_Udp_Ipv4 {
            @display("p=302.51,288.99002");
        }
        ServiceClient1: MyStandardHost_Udp_Ipv4 {
            @display("p=302.51,449.54004");
        }
        ServiceClient2: MyStandardHost_Udp_Ipv4 {
            @display("p=302.51,620.23004");
        }

        Server0: MyStandardHost_Udp_Ipv4 {
            @display("p=1243.8401,288.99002");
        }
        Server1: MyStandardHost_Udp_Ipv4 {
            @display("p=1243.8401,449.54004");
        }
        Server2: MyStandardHost_Udp_Ipv4 {
            @display("p=1243.8401,620.23004");
        }

        switch0: EtherSwitch {
            @display("p=452.92004,447.85004");
        }
        switch1: EtherSwitch {
            @display("p=1090.05,447.85004");
        }

        MobileStation: MyRouter_Udp_Ipv4_Wireless_Station {
            @display("p=620.23004,449.54004");
        }
        ShoreStation: MyRouter_Udp_Ipv4_Wireless_Station {
            @display("p=946.4001,449.54004");
        }
    connections:
        MobileStation.ethg++ <--> Eth100M <--> switch0.ethg++;
        ServiceClient0.ethg++ <--> Eth100M <--> switch0.ethg++;
        ServiceClient1.ethg++ <--> Eth100M <--> switch0.ethg++;
        ServiceClient2.ethg++ <--> Eth100M <--> switch0.ethg++;

        ShoreStation.ethg++ <--> Eth100M <--> switch1.ethg++;
        Server0.ethg++ <--> Eth100M <--> switch1.ethg++;
        Server1.ethg++ <--> Eth100M <--> switch1.ethg++;
        Server2.ethg++ <--> Eth100M <--> switch1.ethg++;
}
