# omnet++/inet_Service-Id-Table 
Define and implement the basic architecture of service-based network layer protocols for Ipv4 based on INET simulation framework in OMNeT++ simulation environment.
# Version
	Operating system: Windows 10-64
	OMNeT++ 5.6.2
	INET 4.2.5
# Directory Structure
	(ClientId & ServiceId)
	1. ClientId:
		src/inet/networklayer/contract/clientid/ClientId.msg
		src/inet/networklayer/contract/serviceid/ClientId.h
		src/inet/networklayer/contract/serviceid/ClientId.cc
		
	2. ServiceId:
		src/inet/networklayer/contract/serviceid/ServiceId.msg
		src/inet/networklayer/contract/serviceid/ServiceId.h
		src/inet/networklayer/contract/serviceid/ServiceId.cc

	(Ipv4Cid & Ipv4Sid)
	1. ICid:
		src/inet/networklayer/contract/ICid.h
		src/inet/networklayer/contract/ICid.cc
		
	2. Ipv4Cid:
		src/inet/networklayer/ipv4/Ipv4Cid.msg
		src/inet/networklayer/ipv4/Ipv4Cid.h
		src/inet/networklayer/ipv4/Ipv4Cid.cc
		
	3. ICid:
		src/inet/networklayer/contract/ICid.h
		src/inet/networklayer/contract/ICid.cc

	4. Ipv4Sid:
		src/inet/networklayer/ipv4/Ipv4Sid.msg
		src/inet/networklayer/ipv4/Ipv4Sid.h
		src/inet/networklayer/ipv4/Ipv4Sid.cc
		
	(Ipv4CidTable & Ipv4SidTable)
	1. ICidTable:
		src/inet/networklayer/contract/ICidTable.ned
		src/inet/networklayer/contract/ICidTable.h
		src/inet/networklayer/ipv4/IIpv4CidTable.h
		
	2. Ipv4CidTable:
		src/inet/networklayer/ipv4/Ipv4CidTable.ned
		src/inet/networklayer/ipv4/Ipv4CidTable.h
		src/inet/networklayer/ipv4/Ipv4CidTable.cc

	3. ISidTable:
		src/inet/networklayer/contract/ISidTable.ned
		src/inet/networklayer/contract/ISidTable.h
		src/inet/networklayer/ipv4/IIpv4SidTable.h
		
	4. Ipv4SidTable:
		src/inet/networklayer/ipv4/Ipv4SidTable.ned
		src/inet/networklayer/ipv4/Ipv4SidTable.h
		src/inet/networklayer/ipv4/Ipv4SidTable.cc
		
	(Ipv4NodeConfigurator & Ipv4NetworkConfigurator)
	1. Ipv4NodeConfigurator:
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.ned
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.h
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.cc
	
	2. Ipv4NetworkConfigurator:
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.ned
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.h
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.cc
	
	3. NetworkConfiguratorBase
		src/inet/networklayer/configurator/base/NetworkConfiguratorBase.h
		src/inet/networklayer/configurator/base/NetworkConfiguratorBase.cc
		
	(Ipv4ServiceHeader & Ipv4ServiceHeaderSerializer)
	1. Ipv4ServiceHeader:
		src/inet/networklayer/ipv4/Ipv4ServiceHeader.msg
		src/inet/networklayer/ipv4/Ipv4ServiceHeader.cc
		
	2. Ipv4ServiceHeaderSerializer：
		src/inet/networklayer/ipv4/Ipv4ServiceHeaderSerializer.h
		src/inet/networklayer/ipv4/Ipv4ServiceHeaderSerializer.cc

	(Ipv4_MobileStation & Ipv4_ShoreStation)
	1. Ipv4_MobileStation:
		src/inet/networklayer/ipv4/Ipv4_MobileStation.ned
		src/inet/networklayer/ipv4/Ipv4_MobileStation.h
		src/inet/networklayer/ipv4/Ipv4_MobileStation.cc
		
	2. Ipv4_ShoreStation:
		src/inet/networklayer/ipv4/Ipv4_ShoreStation.ned
		src/inet/networklayer/ipv4/Ipv4_ShoreStation.h
		src/inet/networklayer/ipv4/Ipv4_ShoreStation.cc
		
	(Ipv4NetworkLayerMobileStation & Ipv4NetworkLayerShoreStation)
	1. Ipv4NetworkLayerMobileStation:
		src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.ned
		src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.h
		src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.cc
	
	2. Ipv4NetworkLayerShoreStation:
		src/inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.ned
		src/inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.h
		src/inet/networklayer/ipv4/Ipv4NetworkLayerShoreStation.cc
		
	(Node)
		src/inet/node/base/MyApplicationLayerNodeBase_Udp_Ipv4.ned
		src/inet/node/base/MyTransportLayerNodeBase_Udp_Ipv4.ned
		src/inet/node/base/MyNetworkLayerNodeBase_Ipv4_MobileStation.ned
		src/inet/node/base/MyLinkLayerNodeBase.ned
		
        	src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_MobileStation.ned
		src/inet/node/inet/MyRouter_Udp_Ipv4_Wireless_ShoreStation.ned
		src/inet/node/inet/MyStandardHost_Udp_Ipv4.ned

	(Archietecture)
		inet/tutorials/configurator/SoI_Architecture.ini
		inet/tutorials/configurator/SoI_Architecture_v2.ned
		inet/tutorials/configurator/SoI_Architecture_v2.xml
		
	*other source files are attached for debugging 
# Usage
1. Copy the program files in the program folder to the same folder as the complete INET module source file path;
2. If there is no corresponding path in the source file, create a new folder according to the path of the program file in the program folder, and then copy the program into it;
3. Rebuild
# Contributors
* Chuntian Xu(Southeast University, China)
chuntianxu2020@163.com
# Notice
If you want to use this code for further development, please contact me for approval !
