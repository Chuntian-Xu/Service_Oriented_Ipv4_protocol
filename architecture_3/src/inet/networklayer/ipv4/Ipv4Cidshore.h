// src/inet/networklayer/ipv4/Ipv4Cidshore.h

#ifndef __INET_IPV4CIDSHORE_H
#define __INET_IPV4CIDSHORE_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ICidshore.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/clientid/ClientId.h"
#include "inet/linklayer/common/MacAddress.h"  // new added

namespace inet {

class InterfaceEntry;
class IIpv4CidshoreTable;

/**
 * Ipv4CidShore in IIpv4CidShoreTable.
 */
class INET_API Ipv4Cidshore : public cObject, public ICidshore {
  private:
    IIpv4CidshoreTable *rt;
    InterfaceEntry *interfacePtr;    ///< interface
    SourceType sourceType;    ///< manual, etc.
    unsigned int adminDist;    ///< Cisco like administrative distance
    Ipv4Address ipaddr;
    ClientId cid;
    int port;
    MacAddress MMSI;  // new added

  private:
    // copying not supported: following are private and also left undefined
    Ipv4Cidshore(const Ipv4Cidshore& obj);
    Ipv4Cidshore& operator=(const Ipv4Cidshore& obj);

  protected:
    void changed(int fieldCode);

  public:
    Ipv4Cidshore():rt(nullptr),interfacePtr(nullptr),sourceType(MANUAL),adminDist(dUnknown),ipaddr(),cid(),MMSI(){}
    virtual ~Ipv4Cidshore();
    virtual std::string str() const override;
    virtual std::string detailedInfo() const OMNETPP5_CODE(override);

    bool operator==(const Ipv4Cidshore& cidshore) const { return equals(cidshore); }
    bool operator!=(const Ipv4Cidshore& cidshore) const { return !equals(cidshore); }
    bool equals(const Ipv4Cidshore& cidshore) const;

    // ^-^
    /** To be called by the cidshore table when this cidshore is added or removed from it */
    virtual void setCidshoreTable(IIpv4CidshoreTable *rt) {
//        EV << " !!! --> Ipv4Cid.h::setCidshoreTable(IIpv4CidTable *rt) \n";
        this->rt = rt;
    }
    IIpv4CidshoreTable *getCidshoreTable() const { return rt; }

    virtual bool isValid() const { return true; }

    /* operation: set */
    virtual void setIpaddr(Ipv4Address _ipaddr) { if (ipaddr != _ipaddr) { ipaddr = _ipaddr; changed(F_DESTINATION); } }
    virtual void setCid(ClientId _cid) { if (cid != _cid) { cid = _cid; changed(F_NEXTHOP); } }
    virtual void setPort(int _port) override {
//        EV_INFO<<"    --> Ipv4Cidshore.h:: setPort(int _port) override"<<endl;
        if(port != _port) port = _port;
        changed(F_PORT);
    }
    virtual void setMMSI(MacAddress _MMSI) override {
//        EV_INFO<<"    --> Ipv4Cidshore.h:: setMMSI(MacAddress _MMSI) override"<<endl;
        if(MMSI != _MMSI) MMSI = _MMSI;
        changed(F_MMSI);
    } // new added

    void setInterface(InterfaceEntry *_interfacePtr) override { if (interfacePtr != _interfacePtr) { interfacePtr = _interfacePtr; changed(F_IFACE); } }
    void setSourceType(SourceType _source) override { if (sourceType != _source) { sourceType = _source; changed(F_SOURCE); } }

    /* operation: get */
    const char* getSourceTypeAbbreviation() const;
    Ipv4Address getIpaddr() const { return ipaddr; }
    ClientId getCid() const override { return cid; }
    int getPort() const override { return port; }
    MacAddress getMMSI() const override { return MMSI; }  // new added

    InterfaceEntry *getInterface() const override { return interfacePtr; }
    const char *getInterfaceName() const;
    SourceType getSourceType() const override { return sourceType; }
    unsigned int getAdminDist() const { return adminDist; }

    virtual ICidshoreTable *getCidshoreTableAsGeneric() const override;

    virtual void setIpaddr(const L3Address& ipaddr) override { setIpaddr(ipaddr.toIpv4()); }
    virtual void setCid(const L3Address& cid) override { setCid(cid.toClientId()); }
    virtual L3Address getIpaddrAsGeneric() const override { return getIpaddr(); }
    virtual ClientId getCidAsGeneric() const override { return getCid(); }
};

} // namespace inet

#endif // ifndef __INET_IPV4CID_H

