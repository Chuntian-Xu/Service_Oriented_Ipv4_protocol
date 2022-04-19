// src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.ned

package inet.networklayer.configurator.ipv4;

import inet.networklayer.configurator.contract.IIpv4NodeConfigurator;

simple Ipv4NodeConfigurator like IIpv4NodeConfigurator {
    parameters:
        string interfaceTableModule;   // The path to the InterfaceTable module
        string routingTableModule;
        // TODO: eventually rename to networkConfigurator
        string networkConfiguratorModule = default("configurator"); // the absolute path to the Ipv4NetworkConfigurator; use "" if there is no configurator
        bool configureRoutingTable = default(true);     // add routing entries to routing table (uses the configurator module)
        
        string sidTableModule; // new added
        bool configureSidTable = default(true);  // new added
        
        string cidshoreTableModule; // new added
        bool configureCidshoreTable = default(true);  // new added
        
        @display("i=block/cogwheel");
}


//
// This module has one instance per network node, and it acts like a bridge
// between the node and the network's global configurator module,
// ~Ipv4NetworkConfigurator.
//
// ~Ipv4NetworkConfigurator only computes and remembers configuration data
// (assigned IP addresses, static routes, etc), but doesn't configure the nodes'
// routing tables and interfaces accordingly. The actual configuration is
// carried out by this module, at network initialization time and also when
// the node restarts (i.e. after a shutdown or a crash.)
//
// This module is needed because of failure/restart scenarios:
// ~Ipv4NetworkConfigurator would not be able to configure the node after
// a restart, because being a global module, it doesn't know about node
// lifecycle events.
//