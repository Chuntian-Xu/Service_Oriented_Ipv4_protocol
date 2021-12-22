//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.tutorials.configurator;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;

import inet.node.inet.MyStandardHost_Udp_Ipv4;
import inet.node.inet.MyRouter_Udp_Ipv4;
import inet.node.ethernet.EtherSwitch;
import inet.node.ethernet.Eth10M;

network beifenConfiguratorB {
    @display("bgb=4263.1924,2147.08");
    submodules:
        area1lan1host0: MyStandardHost_Udp_Ipv4 {
            @display("p=431.87875,1461.1226");
        }
        area1lan1host1: MyStandardHost_Udp_Ipv4 {
            @display("p=431.87875,1073.6425");
        }
        area1lan1host2: MyStandardHost_Udp_Ipv4 {
            @display("p=431.87875,686.1625");
        }
        area2lan1host0: MyStandardHost_Udp_Ipv4 {
            @display("p=3729.495,686.1625");
        }
        area2lan1host1: MyStandardHost_Udp_Ipv4 {
            @display("p=3729.495,1073.6425");
        }
        area2lan1host2: MyStandardHost_Udp_Ipv4 {
            @display("p=3729.495,1461.1226");
        }
        
        area1switch1: EtherSwitch {
            @display("p=1109.9688,1069.6062");
        }
        area2switch1: EtherSwitch {
            @display("p=3132.13,1069.6062");
        }
        
        area1router: MyRouter_Udp_Ipv4 {
            @display("p=1590.2825,1073.6425");
        }
        area2router: MyRouter_Udp_Ipv4 {
            @display("p=2643.7437,1073.6425");
        }
        backbonerouter: MyRouter_Udp_Ipv4 {
            @display("p=2114.995,1073.6425;i=abstract/router2");
        }
        
        configurator: Ipv4NetworkConfigurator {
            @display("p=268.385,123.87");
        }
        visualizer: IntegratedCanvasVisualizer {
            @display("p=696.76874,123.87");
        }
    connections:
        area2lan1host0.ethg++ <--> Eth10M <--> area2switch1.ethg++;
        area2lan1host1.ethg++ <--> Eth10M <--> area2switch1.ethg++;
        area2lan1host2.ethg++ <--> Eth10M <--> area2switch1.ethg++;
        area1switch1.ethg++ <--> Eth10M <--> area1lan1host0.ethg++;
        area1switch1.ethg++ <--> Eth10M <--> area1lan1host1.ethg++;
        area1switch1.ethg++ <--> Eth10M <--> area1lan1host2.ethg++;
        area1switch1.ethg++ <--> Eth10M <--> area1router.ethg++;
        area2switch1.ethg++ <--> Eth10M <--> area2router.ethg++;
        area2router.ethg++ <--> Eth10M <--> backbonerouter.ethg++;
        area1router.ethg++ <--> Eth10M <--> backbonerouter.ethg++;
}
