// src/inet/networklayer/ipv4/Ipv4SidTable.cc

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

#include "inet/networklayer/ipv4/Ipv4Sid.h"
#include "inet/networklayer/ipv4/Ipv4SidTable.h"

#include "inet/networklayer/ipv4/RoutingTableParser.h"

namespace inet {

using namespace utils;

Define_Module(Ipv4SidTable);

std::ostream& operator<<(std::ostream& os, const Ipv4Sid& e) {
    os << e.str();
    return os;
};

Ipv4SidTable::~Ipv4SidTable() {
    for (auto & elem : sids) delete elem;
}

void Ipv4SidTable::initialize(int stage) {
//    EV_INFO << "!!! --> Ipv4SidTable::initialize(int stage) \n"; // new added
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) { // stage0
//        EV_INFO << "!!! stage0 INITSTAGE_LOCAL !!! --> Ipv4SidTable::initialize(int stage)\n"; // new added
        // get a pointer to the host module and IInterfaceTable
        cModule *host = getContainingNode(this);
//        EV_INFO << "!!! hostName: "<<host->getFullName()<<"\n"; // new added

        ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);

        WATCH_PTRVECTOR(sids);
    }
    else if (stage == INITSTAGE_ROUTER_ID_ASSIGNMENT) {// stage6
//        EV_INFO << "!!! stage6 INITSTAGE_ROUTER_ID_ASSIGNMENT !!! --> Ipv4SidTable::initialize(int stage)\n"; // new added
        cModule *node = findContainingNode(this);
//        EV_INFO << "!!! nodeName: "<<node->getFullName()<<"\n"; // new added
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        isNodeUp = !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
//        EV_INFO << "!!! isNodeUp = "<<isNodeUp<<"\n"; // new added
        if (isNodeUp) {
            // set sidId if param is not "" (==no sidId) or "auto" (in which case we'll
            // do it later in a later stage, after network configurators configured the interfaces)
            const char *sidIdStr = par("sidId");
//            EV_INFO << "!!! sidId: "<< *sidIdStr <<"\n"; // new added
            if (strcmp(sidIdStr, "") && strcmp(sidIdStr, "auto")) {
//                EV_INFO << "!!! sidId = Ipv4Address(sidIdStr) "<<"\n"; // new added
                sidId = Ipv4Address(sidIdStr);
            }
            configureSidId();
        }
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {// stage8
//        EV_INFO << "!!! stage8 INITSTAGE_NETWORK_LAYER !!! --> Ipv4SidTable::initialize(int stage)\n"; // new added
        // we don't use notifications during initialize(), so we do it manually.
//        updateNetmaskRoutes();
    }
}

void Ipv4SidTable::configureSidId() {
//    EV_INFO << "!!! ---> Ipv4SidTable::configureSidId()\n"; // new added
    if (sidId.isUnspecified()) {    // not yet configured
//        EV_INFO << "!!! sidId not yet configured \n"; // new added
        const char *sidIdStr = par("sidId");
//        EV_INFO << "!!! sidId: "<< *sidIdStr<<"\n"; // new added
        if (!strcmp(sidIdStr, "auto")) {// non-"auto" cases already handled earlier
            // choose highest interface address as routerId
//            EV_INFO << "!!! choose highest interface address as sidId"<<"\n"; // new added
//            EV_INFO << "!!! ift->getNumInterfaces(): "<<ift->getNumInterfaces()<<"\n"; // new added
            for (int i = 0; i < ift->getNumInterfaces(); ++i) {
                InterfaceEntry *ie = ift->getInterface(i);
                if (!ie->isLoopback()) {
                    auto ipv4Data = ie->findProtocolData<Ipv4InterfaceData>();
                    if (ipv4Data && ipv4Data->getIPAddress().getInt() > sidId.getInt()) {
                        sidId = ipv4Data->getIPAddress();
                    }
                }
            }
        }
    }
    else {    // already configured
              // if there is no interface with routerId yet, assign it to the loopback address;
              // TODO find out if this is a good practice, in which situations it is useful etc.
//        EV_INFO << "!!! sidId already configured \n"; // new added
        if (getInterfaceByAddress(sidId) == nullptr) {
            InterfaceEntry *lo0 = CHK(ift->findFirstLoopbackInterface());
            auto ipv4Data = lo0->getProtocolData<Ipv4InterfaceData>();
            ipv4Data->setIPAddress(sidId);
            ipv4Data->setNetmask(Ipv4Address::ALLONES_ADDRESS);
        }
    }
}

void Ipv4SidTable::invalidateCache() {
//    EV << " !!! --> Ipv4SidTable::invalidateCache() \n";
    sidCache.clear();
}

void Ipv4SidTable::printSidTable() const {
//    EV << " !!! --> Ipv4SidTable::printSidTable()\n";
    EV << " -- ServiceId table --\n";
    EV << stringf("%-16s %-16s \n", "Ip Address", "ServiceId");

    for (int i = 0; i < getNumSids(); i++) {
        Ipv4Sid *sid = getSid(i);
        EV << stringf("%-16s %-16s\n",
                sid->getIpaddr().isUnspecified() ? "*" : sid->getIpaddr().str().c_str(),
                sid->getSid().isUnspecified() ? "*" : sid->getSid().str().c_str());
    }
    EV << "\n";
}

InterfaceEntry *Ipv4SidTable::getInterfaceByAddress(const Ipv4Address& addr) const {
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

Ipv4Sid *Ipv4SidTable::getSid(int k) const {
    if (k < (int)sids.size()) return sids[k];
    return nullptr;
}

// The 'sids' vector stores the sids in this order.
// The best matching sid should precede the other matching sids,
// so the method should return true if a is better the b.
bool Ipv4SidTable::sidLessThan(const Ipv4Sid *a, const Ipv4Sid *b) const {
    // longer prefixes are better, because they are more specific
    if (a->getIpaddr() != b->getIpaddr()) return a->getIpaddr() < b->getIpaddr();

    // for the same ip address:
    // smaller ServiceId is better (if useAdminDist)
    return a->getSid() < b->getSid();
}

void Ipv4SidTable::internalAddSid(Ipv4Sid *entry) {
//    EV_INFO << "!!! --> Ipv4SidTable::internalAddSid(Ipv4Sid *entry) \n"; // new added
    // add to tables
    auto pos = upper_bound(sids.begin(), sids.end(), entry, SidLessThan(*this));
    sids.insert(pos, entry);
    invalidateCache();
    entry->setSidTable(this);
}

// new added ^-^
void Ipv4SidTable::addSid(Ipv4Sid *entry) {
//    EV_INFO << "!!! --> Ipv4SidTable::addSid(Ipv4Sid *entry) \n"; // new added
    Enter_Method("addSid(...)");
    // This method should be called before calling entry->str()
    internalAddSid(entry);
    EV_INFO << "add sid " << entry->str() << "\n";
    emit(sidAddedSignal, entry);
}

Ipv4Sid *Ipv4SidTable::internalRemoveSid(Ipv4Sid *entry) {
    auto i = std::find(sids.begin(), sids.end(), entry);
    if (i != sids.end()) {
        sids.erase(i);
        invalidateCache();
        return entry;
    }
    return nullptr;
}

void Ipv4SidTable::ipv4SidChanged(Ipv4Sid *entry, int fieldCode) {
    if (fieldCode == Ipv4Sid::F_DESTINATION || fieldCode == Ipv4Sid::F_PREFIX_LENGTH || fieldCode == Ipv4Sid::F_METRIC) {    // our data structures depend on these fields
        entry = internalRemoveSid(entry);
        ASSERT(entry != nullptr);    // failure means inconsistency: route was not found in this routing table
        internalAddSid(entry);
    }
    emit(routeChangedSignal, entry);    // TODO include fieldCode in the notification
}

bool Ipv4SidTable::handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback) {
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




