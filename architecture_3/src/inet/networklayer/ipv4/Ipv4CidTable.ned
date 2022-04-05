// src/inet/networklayer/ipv4/Ipv4CidTable.ned

package inet.networklayer.ipv4;

import inet.networklayer.contract.ICidTable;

simple Ipv4CidTable like ICidTable {
    parameters:
        string interfaceTableModule;   // The path to the InterfaceTable module
        string cidId = default("auto");
        string cidFile = default("");  // cid table file name
        
        @signal[cidAdded](type=inet::Ipv4Cid);
        @signal[cidDeleted](type=inet::Ipv4Cid);
        @signal[cidChanged](type=inet::Ipv4Cid);
        
        @display("i=block/table");
}

// Stores the Ipv4 ClientId table.
