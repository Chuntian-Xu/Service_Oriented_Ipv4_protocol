// src/inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.cc

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/networklayer/configurator/ipv4/Ipv4NodeConfigurator.h"

namespace inet {

Define_Module(Ipv4NodeConfigurator);

Ipv4NodeConfigurator::Ipv4NodeConfigurator() { // constructor
    nodeStatus = nullptr;
    interfaceTable = nullptr;
    routingTable = nullptr;
    networkConfigurator = nullptr;

    sidTable = nullptr;// new added
}

// ^-^ 被调用
void Ipv4NodeConfigurator::initialize(int stage) {
//    EV_INFO<<"!!! --> Ipv4NodeConfigurator::initialize(int stage) \n";  // new added
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {  // stage0
//        EV_INFO << "!!! stage0 INITSTAGE_LOCAL !!! --> Ipv4NodeConfigurator::initialize(int stage) \n"; // new added
        cModule *node = getContainingNode(this); // 获取在该stage下配置的node
//        EV_INFO << "!!! nodeName: "<<node->getFullName()<<"\n"; // new added
        const char *networkConfiguratorPath = par("networkConfiguratorModule");// 获取在该stage下的配置module路径
//        EV_INFO << "!!! networkConfiguratorPath: "<<networkConfiguratorPath<<"\n";// new added
        nodeStatus = dynamic_cast<NodeStatus *>(node->getSubmodule("status"));

        interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);  // !!! 获取interfaceTableModule
        routingTable = getModuleFromPar<IIpv4RoutingTable>(par("routingTableModule"), this);  // !!! 获取routingTableModule
//        EV_INFO<<"!!! get interfaceTable: "<<interfaceTable->getFullPath() <<"\n";  // new added
//        EV_INFO<<"!!! get routingTable: "<<routingTable->getNumRoutes() <<"\n";  // new added 理解为什么要设置IIpv4RoutingTable.h这个文件？

        sidTable = getModuleFromPar<IIpv4SidTable>(par("sidTableModule"), this);  // !!! 获取sidTableModule
//        EV_INFO<<"!!! get sidTable" <<"\n";  // new added

        if (!networkConfiguratorPath[0]) networkConfigurator = nullptr;
        else {
            cModule *module = getModuleByPath(networkConfiguratorPath); // 获取在该stage的配置module
//            EV_INFO << "!!! ModuleName: "<< module->getFullName() <<"\n"; // new added
            if (!module) throw cRuntimeError("Configurator module '%s' not found (check the 'networkConfiguratorModule' parameter)", networkConfiguratorPath);
            networkConfigurator = check_and_cast<Ipv4NetworkConfigurator *>(module);
        }
    }
    else if (stage == INITSTAGE_NETWORK_CONFIGURATION) { // !!! stage4
//        EV_INFO<<"!!! stage4 INITSTAGE_NETWORK_CONFIGURATION !!! --> Ipv4NodeConfigurator::initialize(int stage) \n";  // new added
        if (!nodeStatus || nodeStatus->getState() == NodeStatus::UP) prepareAllInterfaces();
    }
    else if (stage == INITSTAGE_NETWORK_ADDRESS_ASSIGNMENT) { // !!! stage5
//        EV_INFO<<"!!! stage5 INITSTAGE_NETWORK_ADDRESS_ASSIGNMENT !!! --> Ipv4NodeConfigurator::initialize(int stage) \n";  // new added
        if ((!nodeStatus || nodeStatus->getState() == NodeStatus::UP) && networkConfigurator)
            configureAllInterfaces();
    }
    else if (stage == INITSTAGE_STATIC_ROUTING) { // !!! stage7
//        EV_INFO<<"!!! stage7 INITSTAGE_STATIC_ROUTING !!! --> Ipv4NodeConfigurator::initialize(int stage) \n";  // new added
        if ((!nodeStatus || nodeStatus->getState() == NodeStatus::UP) && networkConfigurator) {
            configureRoutingTable();
            configureSidTable(); // new added
            cModule *node = getContainingNode(this);
            // get a pointer to the host module and IInterfaceTable
            node->subscribe(interfaceCreatedSignal, this);
            node->subscribe(interfaceDeletedSignal, this);
            node->subscribe(interfaceStateChangedSignal, this);
        }
    }
}

bool Ipv4NodeConfigurator::handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback) {
    Enter_Method_Silent();
    int stage = operation->getCurrentStage();
    if (dynamic_cast<ModuleStartOperation *>(operation)) {
        if (static_cast<ModuleStartOperation::Stage>(stage) == ModuleStartOperation::STAGE_LINK_LAYER)
            prepareAllInterfaces();
        else if (static_cast<ModuleStartOperation::Stage>(stage) == ModuleStartOperation::STAGE_NETWORK_LAYER) {
            if (networkConfigurator != nullptr) {
                configureAllInterfaces();
                configureRoutingTable();
            }
            cModule *node = getContainingNode(this);
            node->subscribe(interfaceCreatedSignal, this);
            node->subscribe(interfaceDeletedSignal, this);
            node->subscribe(interfaceStateChangedSignal, this);
        }
    }
    else if (dynamic_cast<ModuleStopOperation *>(operation)) {
        if (static_cast<ModuleStopOperation::Stage>(stage) == ModuleStopOperation::STAGE_LOCAL) {
            cModule *node = getContainingNode(this);
            node->unsubscribe(interfaceCreatedSignal, this);
            node->unsubscribe(interfaceDeletedSignal, this);
            node->unsubscribe(interfaceStateChangedSignal, this);
        }
    }
    else if (dynamic_cast<ModuleCrashOperation *>(operation)) {
        cModule *node = getContainingNode(this);
        node->unsubscribe(interfaceCreatedSignal, this);
        node->unsubscribe(interfaceDeletedSignal, this);
        node->unsubscribe(interfaceStateChangedSignal, this);
    }
    else
        throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}

// ^-^ 被调用
void Ipv4NodeConfigurator::prepareAllInterfaces() {
//    EV_INFO<<"!!! --> Ipv4NodeConfigurator::prepareAllInterfaces() \n"; // new added
    int NumInterfaces = interfaceTable->getNumInterfaces();//获取接口个数
//    EV_INFO<<"!!! NumInterfaces: "<<NumInterfaces<<"\n"; // new added
    // 遍历并配置每个接口
    for (int i = 0; i < NumInterfaces; i++) prepareInterface(interfaceTable->getInterface(i));
}
// ^-^ 被调用
void Ipv4NodeConfigurator::prepareInterface(InterfaceEntry *interfaceEntry) {
//    EV_INFO<<"!!! --> Ipv4NodeConfigurator::prepareInterface(InterfaceEntry *interfaceEntry) \n"; // new added
    // ASSERT(!interfaceEntry->getProtocolData<Ipv4InterfaceData>());
    Ipv4InterfaceData *interfaceData = interfaceEntry->addProtocolData<Ipv4InterfaceData>();
    if (interfaceEntry->isLoopback()) {
        // we may reconfigure later it to be the routerId
        interfaceData->setIPAddress(Ipv4Address::LOOPBACK_ADDRESS);
        interfaceData->setNetmask(Ipv4Address::LOOPBACK_NETMASK);
        interfaceData->setMetric(1);
    }
    else {
        auto datarate = interfaceEntry->getDatarate();
        // TODO: KLUDGE: how do we set the metric correctly for both wired and wireless interfaces even if datarate is unknown
        if (datarate == 0) interfaceData->setMetric(1);
        else // metric: some hints: OSPF cost (2e9/bps value), MS KB article Q299540, ...
            interfaceData->setMetric((int)ceil(2e9 / datarate));    // use OSPF cost as default
        if (interfaceEntry->isMulticast()) {
            interfaceData->joinMulticastGroup(Ipv4Address::ALL_HOSTS_MCAST);
            if (routingTable->isForwardingEnabled())
                interfaceData->joinMulticastGroup(Ipv4Address::ALL_ROUTERS_MCAST);
        }
    }
}
// ^-^ 被调用
void Ipv4NodeConfigurator::configureAllInterfaces() {
//    EV_INFO<<"!!! --> Ipv4NodeConfigurator::configureAllInterfaces()\n"; // new added
    ASSERT(networkConfigurator);
    int NumInterfaces = interfaceTable->getNumInterfaces();
//    EV_INFO<<"!!! NumInterfaces: "<<NumInterfaces<<"\n"; // new added
    for (int i = 0; i < NumInterfaces; i++) networkConfigurator->configureInterface(interfaceTable->getInterface(i));
}

// ^-^ 被调用
void Ipv4NodeConfigurator::configureRoutingTable() {
//    EV_INFO<<"!!! --> Ipv4NodeConfigurator::configureRoutingTable()\n"; // new added
    ASSERT(networkConfigurator);
    if (par("configureRoutingTable")) networkConfigurator->configureRoutingTable(routingTable);
}

// new added ^-^
void Ipv4NodeConfigurator::configureSidTable() {
//    EV_INFO<<"\n !!! --> Ipv4NodeConfigurator::configureSidTable()\n"; // new added
    ASSERT(networkConfigurator);
    if (par("configureSidTable")) networkConfigurator->configureSidTable(sidTable);
}


void Ipv4NodeConfigurator::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (getSimulation()->getContextType() == CTX_INITIALIZE)
        return; // ignore notifications during initialize

    Enter_Method_Silent();
    printSignalBanner(signalID, obj, details);

    if (signalID == interfaceCreatedSignal) {
        auto *entry = check_and_cast<InterfaceEntry *>(obj);
        prepareInterface(entry);
        // TODO
    }
    else if (signalID == interfaceDeletedSignal) {
        // The RoutingTable deletes routing entries of interface
    }
    else if (signalID == interfaceStateChangedSignal) {
        const auto *ieChangeDetails = check_and_cast<const InterfaceEntryChangeDetails *>(obj);
        auto fieldId = ieChangeDetails->getFieldId();
        if (fieldId == InterfaceEntry::F_STATE || fieldId == InterfaceEntry::F_CARRIER) {
            auto *entry = ieChangeDetails->getInterfaceEntry();
            if (entry->isUp() && networkConfigurator) {
                networkConfigurator->configureInterface(entry);
                if (par("configureRoutingTable"))
                    networkConfigurator->configureRoutingTable(routingTable, entry);
            }
            // otherwise the RoutingTable deletes routing entries of interface entry
        }
    }
}

} // namespace inet

