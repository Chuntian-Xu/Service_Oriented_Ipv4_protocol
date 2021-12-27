# omnet++/inet_Service-Id-Table 
Implement Ipv4 ServiceId Table(&lt;ipaddr,sid>) based on INET framework on OMNeT++.

# 软件版本: 
	OMNeT++ 5.6.2(https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.6.2/omnetpp-5.6.2-src-windows.zip)
	INET 4.2.5(https://github.com/inet-framework/inet/releases/download/v4.2.5/inet-4.2.5-src.tgz)

# 程序结构:
	1. ISidTable模块(SidTable 接口模块):
		src/inet/networklayer/contract/ISidTable.ned
		src/inet/networklayer/contract/ISidTable.h
		src/inet/networklayer/contract/ISid.h
		src/inet/networklayer/contract/ISid.cc
		src/inet/networklayer/ipv4/IIpv4SidTable.h

	2. ServiceId消息模块(用于规定,表示Service Id 的数据格式,并对其进行各种操作):
		src/inet/networklayer/contract/serviceid/ServiceId.msg
		src/inet/networklayer/contract/serviceid/ServiceId.h
		src/inet/networklayer/contract/serviceid/ServiceId.cc

	3. Ipv4Sid消息模块(用于规定和表示Service Id Table 中所存储的<ipaddr,sid>映射的数据格式,并对其进行各种操作):
		src/inet/networklayer/ipv4/Ipv4Sid.ned
		src/inet/networklayer/ipv4/Ipv4Sid.h
		src/inet/networklayer/ipv4/Ipv4Sid.cc
		
	4. Ipv4SidTable模块(实现Service Id Table,并对其进行各种操作):
		src/inet/networklayer/ipv4/Ipv4SidTable.ned
		src/inet/networklayer/ipv4/Ipv4SidTable.h
		src/inet/networklayer/ipv4/Ipv4SidTable.cc
	
	5. Ipv4NodeConfigurator和Ipv4NetworkConfigurator模块(在初始化时配置网络和网络节点)
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.ned
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.h
		src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.cc
		
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.ned
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.h
		src/inet/networklayer/configurator/ipv4/Ipv4NetworkConfigurator.cc

# 程序使用方法:
	1. 将程序文件夹中的程序文件拷贝到完整的INET模块源文件相同路径的文件夹下;
	2. 如果源文件中没有对应的路径,则根据程序文件夹中程序文件所在路径新建文件夹,然后将程序拷贝进去;
	3. 重新build整个源文件
	4. 运行文件tutorials/configurator/beifen_omnetpp_udp.ini,即可在输出的log中看到打印出来的Service Id Table 


# 声明: 如需引用程序, 请先征求作者同意
