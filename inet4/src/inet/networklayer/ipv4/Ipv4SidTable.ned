// src/inet/networklayer/ipv4/Ipv4SidTable.ned

package inet.networklayer.ipv4;

import inet.networklayer.contract.ISidTable;

simple Ipv4SidTable like ISidTable {
    parameters:
        string interfaceTableModule;   // The path to the InterfaceTable module
        
        string sidFile = default("");  // sid table file name
        
        @display("i=block/table");
        @signal[sidAdded](type=inet::Ipv4Sid);
        @signal[sidDeleted](type=inet::Ipv4Sid);
        @signal[sidChanged](type=inet::Ipv4Sid);
}

// Stores the Ipv4 ServiceId table.
