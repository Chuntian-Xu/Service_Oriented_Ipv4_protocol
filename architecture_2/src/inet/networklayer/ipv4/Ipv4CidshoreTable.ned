// src/inet/networklayer/ipv4/Ipv4CidshoreTable.ned

package inet.networklayer.ipv4;

import inet.networklayer.contract.ICidshoreTable;

simple Ipv4CidshoreTable like ICidshoreTable {
    parameters:
        string interfaceTableModule;   // The path to the InterfaceTable module
        string cidshoreId = default("auto");
        string cidshoreFile = default("");  // cidshore table file name
        
        @signal[cidshoreAdded](type=inet::Ipv4Cidshore);
        @signal[cidshoreDeleted](type=inet::Ipv4Cidshore);
        @signal[cidshoreChanged](type=inet::Ipv4Cidshore);
        
        @display("i=block/table");
}

// Stores the Ipv4_ClientId_shore_table.
