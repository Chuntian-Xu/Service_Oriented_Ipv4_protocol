// /inet/tutorials/configurator/beifenConfiguratorA.ned
 
package inet.tutorials.configurator;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;

import inet.node.inet.MyStandardHost_Udp_Ipv4;
import inet.node.inet.MyRouter_Udp_Ipv4;
import inet.node.ethernet.EtherSwitch;

import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10M;

network beifenConfiguratorA {
    @display("bgb=1610.5125,874.96497");
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=129.12,98.8575");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=129.12,240.08249");
        }

        host0: MyStandardHost_Udp_Ipv4 {
            @display("p=363.15,292.5375");
        }
        host1: MyStandardHost_Udp_Ipv4 {
            @display("p=363.15,449.9025");
        }
        host2: MyStandardHost_Udp_Ipv4 {
            @display("p=363.15,625.425");
        }
        host3: MyStandardHost_Udp_Ipv4 {
            @display("p=1295.235,292.5375");
        }
        host4: MyStandardHost_Udp_Ipv4 {
            @display("p=1295.235,451.91998");
        }
        host5: MyStandardHost_Udp_Ipv4 {
            @display("p=1295.235,625.425");
        }

        router0: MyRouter_Udp_Ipv4 {
            @display("p=667.7925,449.9025");
        }
        router1: MyRouter_Udp_Ipv4 {
            @display("p=960.32996,449.9025");
        }
        router2: MyRouter_Udp_Ipv4 {
            @display("p=815.07,449.9025");
        }

        switch0: EtherSwitch {
            @display("p=518.4975,449.9025");
        } 
        switch1: EtherSwitch {
            @display("p=1099.5375,449.9025");
        }

    connections:
        host2.ethg++ <--> Eth100M <--> switch0.ethg++;
        host1.ethg++ <--> Eth100M <--> switch0.ethg++;
        host0.ethg++ <--> Eth100M <--> switch0.ethg++;
        switch0.ethg++ <--> Eth100M <--> router0.ethg++;
        router0.ethg++ <--> Eth100M <--> router2.ethg++;
        router1.ethg++ <--> Eth100M <--> router2.ethg++;
        switch1.ethg++ <--> Eth100M <--> host3.ethg++;
        switch1.ethg++ <--> Eth100M <--> host4.ethg++;
        switch1.ethg++ <--> Eth100M <--> host5.ethg++;
        router1.ethg++ <--> Eth100M <--> switch1.ethg++;
}