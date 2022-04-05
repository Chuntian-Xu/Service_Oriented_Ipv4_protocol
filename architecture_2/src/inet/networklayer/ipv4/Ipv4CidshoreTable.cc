// src/inet/networklayer/ipv4/Ipv4CidshoreTable.cc

#include <algorithm>
#include <sstream>

#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/PatternMatcher.h"
#include "inet/common/Simsignals.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"

#include "inet/networklayer/ipv4/Ipv4Cidshore.h"
#include "inet/networklayer/ipv4/Ipv4CidshoreTable.h"

#include "inet/networklayer/ipv4/RoutingTableParser.h"

namespace inet {

using namespace utils;

Define_Module(Ipv4CidshoreTable);

std::ostream& operator<<(std::ostream& os, const Ipv4Cidshore& ipv4Cidshore) {
    os << ipv4Cidshore.str();
    return os;
};

Ipv4CidshoreTable::~Ipv4CidshoreTable() {
    for (auto & elem : cidshores) delete elem;
}

void Ipv4CidshoreTable::initialize(int stage) {
    EV_INFO << "!!! --> Ipv4CidshoreTable::initialize(int stage) \n"; // new added
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) { // stage0
        EV_INFO << "    --> stage0 INITSTAGE_LOCAL !!! --> Ipv4CidshoreTable::initialize(int stage)\n"; // new added
        // get a pointer to the host module and IInterfaceTable
        cModule *host = getContainingNode(this);
        EV_INFO << "    --> hostName: "<<host->getFullName()<<"\n"; // new added

        ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);

        WATCH_PTRVECTOR(cidshores);
    }
    else if (stage == INITSTAGE_ROUTER_ID_ASSIGNMENT) {// stage6
        EV_INFO << "    --> stage6 INITSTAGE_ROUTER_ID_ASSIGNMENT !!! --> Ipv4CidshoreTable::initialize(int stage)\n"; // new added
        cModule *node = findContainingNode(this);
        EV_INFO << "    --> nodeName: "<<node->getFullName()<<"\n"; // new added
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        isNodeUp = !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
        EV_INFO << "    --> isNodeUp = "<<isNodeUp<<"\n"; // new added
        if (isNodeUp) {
            // set cidshoreId if param is not "" (==no cidshoreId) or "auto" (in which case we'll
            // do it later in a later stage, after network configurators configured the interfaces)
            const char *cidshoreIdStr = par("cidshoreId");
            EV_INFO << "    --> cidshoreId: "<< *cidshoreIdStr <<"\n"; // new added
            if (strcmp(cidshoreIdStr, "") && strcmp(cidshoreIdStr, "auto")) {
                EV_INFO << "    --> cidshoreId = Ipv4Address(cidshoreIdStr) "<<"\n"; // new added
                cidshoreId = Ipv4Address(cidshoreIdStr);
            }
            configureCidshoreId();
        }
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {// stage8
        EV_INFO << "    --> stage8 INITSTAGE_NETWORK_LAYER !!! --> Ipv4CidshoreTable::initialize(int stage)\n"; // new added
        // we don't use notifications during initialize(), so we do it manually.
    }
}

void Ipv4CidshoreTable::configureCidshoreId() {
    EV_INFO << "!!! ---> Ipv4CidshoreTable::configureCidshoreId()\n"; // new added
    if (cidshoreId.isUnspecified()) {    // not yet configured
        EV_INFO << "    --> cidshoreId not yet configured \n"; // new added
        const char *cidshoreIdStr = par("cidshoreId");
        EV_INFO << "    --> cidshoreId: "<< *cidshoreIdStr<<"\n"; // new added
        if (!strcmp(cidshoreIdStr, "auto")) {// non-"auto" cases already handled earlier
            // choose highest interface address as routerId
            EV_INFO << "    --> choose highest interface address as cidshoreId"<<"\n"; // new added
            EV_INFO << "    --> ift->getNumInterfaces(): "<<ift->getNumInterfaces()<<"\n"; // new added
            for (int i = 0; i < ift->getNumInterfaces(); ++i) {
                InterfaceEntry *ie = ift->getInterface(i);
                if (!ie->isLoopback()) {
                    auto ipv4Data = ie->findProtocolData<Ipv4InterfaceData>();
                    if (ipv4Data && ipv4Data->getIPAddress().getInt() > cidshoreId.getInt()) {
                        cidshoreId = ipv4Data->getIPAddress();
                    }
                }
            }
        }
    }
    else {    // already configured
              // if there is no interface with routerId yet, assign it to the loopback address;
              // TODO find out if this is a good practice, in which situations it is useful etc.
        EV_INFO << "    --> cidshoreId already configured \n"; // new added
        if (getInterfaceByAddress(cidshoreId) == nullptr) {
            InterfaceEntry *lo0 = CHK(ift->findFirstLoopbackInterface());
            auto ipv4Data = lo0->getProtocolData<Ipv4InterfaceData>();
            ipv4Data->setIPAddress(cidshoreId);
            ipv4Data->setNetmask(Ipv4Address::ALLONES_ADDRESS);
        }
    }
}

void Ipv4CidshoreTable::invalidateCache() {
//    EV << " !!! --> Ipv4CidshoreTable::invalidateCache() \n";
    cidshoreCache.clear();
}

void Ipv4CidshoreTable::printCidshoreTable() const {
//    EV << " !!! --> Ipv4CidshoreTable::printCidshoreTable()\n";
    EV << " -- ClientId_shore table --\n";
    EV << stringf("%-16s %4s %-16s\n", "IpAddress", "Port", "MMSI");

    for (int i = 0; i < getNumCidshores(); i++) {
        Ipv4Cidshore *cidshore = getCidshore(i);
        EV << stringf("%-16s %-6d %-16s\n",
                cidshore->getIpaddr().isUnspecified() ? "*" : cidshore->getIpaddr().str().c_str(),
                cidshore->getPort(),
                cidshore->getMMSI().isUnspecified() ? "*" : cidshore->getMMSI().str().c_str());
    }
    EV << "\n";
}

InterfaceEntry *Ipv4CidshoreTable::getInterfaceByAddress(const Ipv4Address& addr) const {
    Enter_Method("getInterfaceByAddress(%u.%u.%u.%u)", addr.getDByte(0), addr.getDByte(1), addr.getDByte(2), addr.getDByte(3));    // note: str().c_str() too slow here

    if (addr.isUnspecified())
        return nullptr;
    for (int i = 0; i < ift->getNumInterfaces(); ++i) {
        InterfaceEntry *ie = ift->getInterface(i);
        if (ie->hasNetworkAddress(addr))
            return ie;
    }
    return nullptr;
}

Ipv4Cidshore *Ipv4CidshoreTable::getCidshore(int k) const {
    if (k < (int)cidshores.size()) return cidshores[k];
    return nullptr;
}

// The 'cidshores' vector stores the cids in this order.
// The best matching cid should precede the other matching cids,
// so the method should return true if a is better the b.
bool Ipv4CidshoreTable::cidshoreLessThan(const Ipv4Cidshore *a, const Ipv4Cidshore *b) const {
    // longer prefixes are better, because they are more specific
    return a->getIpaddr() < b->getIpaddr();
}

void Ipv4CidshoreTable::internalAddCidshore(Ipv4Cidshore *ipv4Cidshore) {
//    EV_INFO << "!!! --> Ipv4CidshoreTable::internalAddCidshore(Ipv4Cidshore *entry) \n"; // new added
    // add to tables
    auto pos = upper_bound(cidshores.begin(), cidshores.end(), ipv4Cidshore, CidshoreLessThan(*this));
    cidshores.insert(pos, ipv4Cidshore);
    invalidateCache();
    ipv4Cidshore->setCidshoreTable(this);
}

void Ipv4CidshoreTable::addCidshore(Ipv4Cidshore *ipv4Cidshore) {
//    EV_INFO << "!!! --> Ipv4CidshoreTable::addCidshore(Ipv4Cidshore *entry) \n"; // new added
    Enter_Method("addCidshore(...)");
    // This method should be called before calling entry->str()
    internalAddCidshore(ipv4Cidshore);
    EV_INFO << "add cidshore " << ipv4Cidshore->str() << "\n";
    emit(cidshoreAddedSignal, ipv4Cidshore);
}

Ipv4Cidshore *Ipv4CidshoreTable::internalRemoveCidshore(Ipv4Cidshore *ipv4Cidshore) {
    auto i = std::find(cidshores.begin(), cidshores.end(), ipv4Cidshore);
    if (i != cidshores.end()) {
        cidshores.erase(i);
        invalidateCache();
        return ipv4Cidshore;
    }
    return nullptr;
}

void Ipv4CidshoreTable::ipv4CidshoreChanged(Ipv4Cidshore *ipv4Cidshore, int fieldCode) {
    if (fieldCode == Ipv4Cidshore::F_DESTINATION || fieldCode == Ipv4Cidshore::F_PREFIX_LENGTH || fieldCode == Ipv4Cidshore::F_METRIC|| fieldCode == Ipv4Cidshore::F_PORT) {    // our data structures depend on these fields
        ipv4Cidshore = internalRemoveCidshore(ipv4Cidshore);
        ASSERT(ipv4Cidshore != nullptr);    // failure means inconsistency: route was not found in this routing table
        internalAddCidshore(ipv4Cidshore);
    }
    emit(cidshoreChangedSignal, ipv4Cidshore);    // TODO include fieldCode in the notification
}

bool Ipv4CidshoreTable::handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback) {
    Enter_Method_Silent();
    int stage = operation->getCurrentStage();
    if (dynamic_cast<ModuleStartOperation *>(operation)) {
        if (static_cast<ModuleStartOperation::Stage>(stage) == ModuleStartOperation::STAGE_NETWORK_LAYER) {
            // read routing table file (and interface configuration)
            const char *filename = par("cidshoreFile");
//            RoutingTableParser parser(ift, this);
//            if (*filename && parser.readRoutingTableFromFile(filename) == -1) throw cRuntimeError("Error reading routing table file %s", filename);
        }
        else if (static_cast<ModuleStartOperation::Stage>(stage) == ModuleStartOperation::STAGE_TRANSPORT_LAYER) {
//            configureRouterId();
//            updateNetmaskRoutes();
            isNodeUp = true;
        }
    }
    else if (dynamic_cast<ModuleStopOperation *>(operation)) {
        if (static_cast<ModuleStopOperation::Stage>(stage) == ModuleStopOperation::STAGE_NETWORK_LAYER) {
//            while (!routes.empty()) delete removeRoute(routes[0]);
            isNodeUp = false;
        }
    }
    else if (dynamic_cast<ModuleCrashOperation *>(operation)) {
        if (static_cast<ModuleCrashOperation::Stage>(stage) == ModuleCrashOperation::STAGE_CRASH) {
//            while (!routes.empty()) delete removeRoute(routes[0]);
            isNodeUp = false;
        }
    }
    return true;
}

} // namespace inet




