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
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;

import inet.node.ethernet.EtherSwitch;
import inet.node.inet.MyRouter_Udp_Ipv4;
import inet.node.wireless.AccessPoint;
import inet.node.inet.MyWirelessHost_Udp_Ipv4;

import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10M;

network beifenConfiguratorC
{
    @display("bgb=959.74756,483.5");
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=60.4375,37.471252");
        }
        visualizer: IntegratedCanvasVisualizer {

            @display("p=171.6425,37.471252");
        }
        
        area1lan1host1: MyWirelessHost_Udp_Ipv4 {
            @display("p=158.34625,147.4675");
        }
        area1lan1host2: MyWirelessHost_Udp_Ipv4 {
            @display("p=152.3025,238.12376");
        }
        area1lan1host3: MyWirelessHost_Udp_Ipv4 {
            @display("p=158.34625,329.98877");
        }
        area2lan1host1: MyWirelessHost_Udp_Ipv4 {
            @display("p=829.2025,147.4675");
        }
        area2lan1host2: MyWirelessHost_Udp_Ipv4 {
            @display("p=829.2025,229.6625");
        }
        area2lan1host3: MyWirelessHost_Udp_Ipv4 {
            @display("p=829,329");
        }

        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=281.63876,39.888752");
        }

        area1router: MyRouter_Udp_Ipv4 {
            @display("p=379.54752,229.6625");
        }
        area2router: MyRouter_Udp_Ipv4 {
            @display("p=595.91376,229.6625");
        }
        backbonerouter: MyRouter_Udp_Ipv4 {
            @display("p=489,250;i=abstract/router2");
        }
        
        area1AP: AccessPoint {
            @display("p=274.38626,236.91501");
        }
        area2AP: AccessPoint {
            @display("p=700,236");
        }

    connections:
        area2router.ethg++ <--> Eth10M <--> backbonerouter.ethg++;
        area1router.ethg++ <--> Eth10M <--> backbonerouter.ethg++;

        area1router.ethg++ <--> Eth10M <--> area1AP.ethg++;
        area2router.ethg++ <--> Eth10M <--> area2AP.ethg++;
}

