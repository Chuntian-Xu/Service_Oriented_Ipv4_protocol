# omnet++/inet_Service-Id-Table 
Implement Ipv4 ServiceId Table(&lt;ipaddr,sid>) based on INET framework on OMNeT++.

# Version: 
	OMNeT++ 5.6.2(https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.6.2/omnetpp-5.6.2-src-windows.zip)
	INET 4.2.5(https://github.com/inet-framework/inet/releases/download/v4.2.5/inet-4.2.5-src.tgz)

# Framework:
	(To implement the function of ServiceId Table)
	1. ISidTable:
		src/inet/networklayer/contract/ISidTable.ned
		src/inet/networklayer/contract/ISidTable.h
		src/inet/networklayer/contract/ISid.h
		src/inet/networklayer/contract/ISid.cc
		src/inet/networklayer/ipv4/IIpv4SidTable.h

	2. ServiceId:
		src/inet/networklayer/contract/serviceid/ServiceId.msg
		src/inet/networklayer/contract/serviceid/ServiceId.h
		src/inet/networklayer/contract/serviceid/ServiceId.cc

	3. Ipv4Sid:
		src/inet/networklayer/ipv4/Ipv4Sid.msg
		src/inet/networklayer/ipv4/Ipv4Sid.h
		src/inet/networklayer/ipv4/Ipv4Sid.cc
		
	4. Ipv4SidTable:
		src/inet/networklayer/ipv4/Ipv4SidTable.ned
		src/inet/networklayer/ipv4/Ipv4SidTable.h
		src/inet/networklayer/ipv4/Ipv4SidTable.cc
		
	(To Configure the ServiceId Table)
	5. Ipv4NodeConfigurator & Ipv4NetworkConfigurator:
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.ned
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.h
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.cc
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.ned
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.h
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.cc
		
	(To implement the function of MobileStation and ShoreStation)
	6. Ipv4NetworkLayerMobileStation & Ipv4NetworkLayerShoreStation:
		 src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.ned
		 src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.h
		 src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.cc
		 src/inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.ned
		 src/inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.h
		 src/inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.cc
		 
	7. Ipv4_MobileStation & Ipv4_ShoreStation:
		src/inet/networklayer/ipv4/Ipv4_MobileStation.ned
		src/inet/networklayer/ipv4/Ipv4_MobileStation.h
		src/inet/networklayer/ipv4/Ipv4_MobileStation.cc
		src/inet/networklayer/ipv4/Ipv4_ShoreStation.ned
		src/inet/networklayer/ipv4/Ipv4_ShoreStation.h
		src/inet/networklayer/ipv4/Ipv4_ShoreStation.cc
	
	8. Ipv4ServiceHeader & Ipv4ServiceHeaderSerializer:
		src/inet/networklayer/ipv4/Ipv4ServiceHeader.msg
		src/inet/networklayer/ipv4/Ipv4ServiceHeader.cc
		src/inet/networklayer/ipv4/Ipv4ServiceHeaderSerializer.h
		src/inet/networklayer/ipv4/Ipv4ServiceHeaderSerializer.cc
                
	(Base Node of Network framework)
	9. Base Node
        	src/inet/node/inet/MyStandardHost_Udp_Ipv4.ned
		src/inet/node/base/MyApplicationLayerNodeBase_Udp_Ipv4.ned
		src/inet/node/base/MyTransportLayerNodeBase_Udp_Ipv4.ned
        	src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_MobileStation.ned
        	src/inet/node/base/MyNetworkLayerNodeBase_Ipv4_MobileStation.ned
        	src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_ShoreStation.ned
        	src/inet/node/base/MyNetworkLayerNodeBase_Ipv4_ShoreStation.ned

# Usage:
1. Copy the program files in the program folder to the same folder as the complete INET module source file path;
2. If there is no corresponding path in the source file, create a new folder according to the path of the program file in the program folder, and then copy the program into it;
3. Rebuild

# Notice: If you want to use this code for further development, please contact the author for approval !
