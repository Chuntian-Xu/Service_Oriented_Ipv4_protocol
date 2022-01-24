// src/inet/networklayer/ipv4/Ipv4CidTable.cc 

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

#include "inet/networklayer/ipv4/Ipv4Cid.h"
#include "inet/networklayer/ipv4/Ipv4CidTable.h"

#include "inet/networklayer/ipv4/RoutingTableParser.h"

namespace inet {

using namespace utils;

Define_Module(Ipv4CidTable);

std::ostream& operator<<(std::ostream& os, const Ipv4Cid& e) {
    os << e.str();
    return os;
};

Ipv4CidTable::~Ipv4CidTable() {
    for (auto & elem : cids) delete elem;
}

void Ipv4CidTable::initialize(int stage) {
//    EV_INFO << "!!! --> Ipv4CidTable::initialize(int stage) \n"; // new added
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) { // stage0
//        EV_INFO << "!!! stage0 INITSTAGE_LOCAL !!! --> Ipv4CidTable::initialize(int stage)\n"; // new added
        // get a pointer to the host module and IInterfaceTable
        cModule *host = getContainingNode(this);
//        EV_INFO << "!!! hostName: "<<host->getFullName()<<"\n"; // new added

        ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);

        WATCH_PTRVECTOR(cids);
    }
    else if (stage == INITSTAGE_ROUTER_ID_ASSIGNMENT) {// stage6
//        EV_INFO << "!!! stage6 INITSTAGE_ROUTER_ID_ASSIGNMENT !!! --> Ipv4CidTable::initialize(int stage)\n"; // new added
        cModule *node = findContainingNode(this);
//        EV_INFO << "!!! nodeName: "<<node->getFullName()<<"\n"; // new added
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        isNodeUp = !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
//        EV_INFO << "!!! isNodeUp = "<<isNodeUp<<"\n"; // new added
        if (isNodeUp) {
            // set cidId if param is not "" (==no cidId) or "auto" (in which case we'll
            // do it later in a later stage, after network configurators configured the interfaces)
            const char *cidIdStr = par("cidId");
//            EV_INFO << "!!! cidId: "<< *cidIdStr <<"\n"; // new added
            if (strcmp(cidIdStr, "") && strcmp(cidIdStr, "auto")) {
//                EV_INFO << "!!! cidId = Ipv4Address(cidIdStr) "<<"\n"; // new added
                cidId = Ipv4Address(cidIdStr);
            }
            configureCidId();
        }
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {// stage8
//        EV_INFO << "!!! stage8 INITSTAGE_NETWORK_LAYER !!! --> Ipv4CidTable::initialize(int stage)\n"; // new added
        // we don't use notifications during initialize(), so we do it manually.
//        updateNetmaskRoutes();
    }
}

void Ipv4CidTable::configureCidId() {
//    EV_INFO << "!!! ---> Ipv4CidTable::configureCidId()\n"; // new added
    if (cidId.isUnspecified()) {    // not yet configured
//        EV_INFO << "!!! cidId not yet configured \n"; // new added
        const char *cidIdStr = par("cidId");
//        EV_INFO << "!!! cidId: "<< *cidIdStr<<"\n"; // new added
        if (!strcmp(cidIdStr, "auto")) {// non-"auto" cases already handled earlier
            // choose highest interface address as routerId
//            EV_INFO << "!!! choose highest interface address as sidId"<<"\n"; // new added
//            EV_INFO << "!!! ift->getNumInterfaces(): "<<ift->getNumInterfaces()<<"\n"; // new added
            for (int i = 0; i < ift->getNumInterfaces(); ++i) {
                InterfaceEntry *ie = ift->getInterface(i);
                if (!ie->isLoopback()) {
                    auto ipv4Data = ie->findProtocolData<Ipv4InterfaceData>();
                    if (ipv4Data && ipv4Data->getIPAddress().getInt() > cidId.getInt()) {
                        cidId = ipv4Data->getIPAddress();
                    }
                }
            }
        }
    }
    else {    // already configured
              // if there is no interface with routerId yet, assign it to the loopback address;
              // TODO find out if this is a good practice, in which situations it is useful etc.
//        EV_INFO << "!!! sidId already configured \n"; // new added
        if (getInterfaceByAddress(cidId) == nullptr) {
            InterfaceEntry *lo0 = CHK(ift->findFirstLoopbackInterface());
            auto ipv4Data = lo0->getProtocolData<Ipv4InterfaceData>();
            ipv4Data->setIPAddress(cidId);
            ipv4Data->setNetmask(Ipv4Address::ALLONES_ADDRESS);
        }
    }
}

void Ipv4CidTable::invalidateCache() {
//    EV << " !!! --> Ipv4CidTable::invalidateCache() \n";
    cidCache.clear();
}

void Ipv4CidTable::printCidTable() const {
//    EV << " !!! --> Ipv4CidTable::printCidTable()\n";
    EV << " -- ClientId table --\n";
    EV << stringf("%-16s %-16s \n", "Ip Address", "ClientId");

    for (int i = 0; i < getNumCids(); i++) {
        Ipv4Cid *cid = getCid(i);
        EV << stringf("%-16s %-16s \n",
                cid->getIpaddr().isUnspecified() ? "*" : cid->getIpaddr().str().c_str(),
                cid->getCid().isUnspecified() ? "*" : cid->getCid().str().c_str());
    }
    EV << "\n";
}

InterfaceEntry *Ipv4CidTable::getInterfaceByAddress(const Ipv4Address& addr) const {
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

Ipv4Cid *Ipv4CidTable::getCid(int k) const {
    if (k < (int)cids.size()) return cids[k];
    return nullptr;
}

// The 'cids' vector stores the cids in this order.
// The best matching cid should precede the other matching cids,
// so the method should return true if a is better the b.
bool Ipv4CidTable::cidLessThan(const Ipv4Cid *a, const Ipv4Cid *b) const {
    // longer prefixes are better, because they are more specific
    if (a->getIpaddr() != b->getIpaddr()) return a->getIpaddr() < b->getIpaddr();

    // for the same ip address:
    // smaller ClientId is better (if useAdminDist)
    return a->getCid() < b->getCid();
}

void Ipv4CidTable::internalAddCid(Ipv4Cid *entry) {
//    EV_INFO << "!!! --> Ipv4CidTable::internalAddCid(Ipv4Cid *entry) \n"; // new added
    // add to tables
    auto pos = upper_bound(cids.begin(), cids.end(), entry, CidLessThan(*this));
    cids.insert(pos, entry);
    invalidateCache();
    entry->setCidTable(this);
}

void Ipv4CidTable::addCid(Ipv4Cid *entry) {
//    EV_INFO << "!!! --> Ipv4CidTable::addCid(Ipv4Cid *entry) \n"; // new added
    Enter_Method("addCid(...)");
    // This method should be called before calling entry->str()
    internalAddCid(entry);
    EV_INFO << "add cid " << entry->str() << "\n";
    emit(cidAddedSignal, entry);
}

Ipv4Cid *Ipv4CidTable::internalRemoveCid(Ipv4Cid *entry) {
    auto i = std::find(cids.begin(), cids.end(), entry);
    if (i != cids.end()) {
        cids.erase(i);
        invalidateCache();
        return entry;
    }
    return nullptr;
}

void Ipv4CidTable::ipv4CidChanged(Ipv4Cid *entry, int fieldCode) {
    if (fieldCode == Ipv4Cid::F_DESTINATION || fieldCode == Ipv4Cid::F_PREFIX_LENGTH || fieldCode == Ipv4Cid::F_METRIC) {    // our data structures depend on these fields
        entry = internalRemoveCid(entry);
        ASSERT(entry != nullptr);    // failure means inconsistency: route was not found in this routing table
        internalAddCid(entry);
    }
    emit(routeChangedSignal, entry);    // TODO include fieldCode in the notification
}

bool Ipv4CidTable::handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback) {
    Enter_Method_Silent();
    int stage = operation->getCurrentStage();
    if (dynamic_cast<ModuleStartOperation *>(operation)) {
        if (static_cast<ModuleStartOperation::Stage>(stage) == ModuleStartOperation::STAGE_NETWORK_LAYER) {
            // read routing table file (and interface configuration)
            const char *filename = par("routingFile");
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




