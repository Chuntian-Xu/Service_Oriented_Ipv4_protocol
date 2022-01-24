// src/inet/networklayer/ipv4/Ipv4Cid.h 

#ifndef __INET_IPV4CID_H
#define __INET_IPV4CID_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ICid.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/clientid/ClientId.h"

namespace inet {

class InterfaceEntry;
class IIpv4CidTable;

/**
 * Ipv4 cid in IIpv4CidTable.
 */
class INET_API Ipv4Cid : public cObject, public ICid {
  private:
    IIpv4CidTable *rt;
    InterfaceEntry *interfacePtr;    ///< interface
    SourceType sourceType;    ///< manual, etc.
    unsigned int adminDist;    ///< Cisco like administrative distance
    Ipv4Address ipaddr;
    ClientId cid;

  private:
    // copying not supported: following are private and also left undefined
    Ipv4Cid(const Ipv4Cid& obj);
    Ipv4Cid& operator=(const Ipv4Cid& obj);

  protected:
    void changed(int fieldCode);

  public:
    Ipv4Cid():rt(nullptr),interfacePtr(nullptr),sourceType(MANUAL),adminDist(dUnknown),ipaddr(),cid(){}
    virtual ~Ipv4Cid();
    virtual std::string str() const override;
    virtual std::string detailedInfo() const OMNETPP5_CODE(override);

    bool operator==(const Ipv4Cid& cid) const { return equals(cid); }
    bool operator!=(const Ipv4Cid& cid) const { return !equals(cid); }
    bool equals(const Ipv4Cid& cid) const;

    // ^-^
    /** To be called by the cid table when this cid is added or removed from it */
    virtual void setCidTable(IIpv4CidTable *rt) {
//        EV << " !!! --> Ipv4Cid.h::setCidTable(IIpv4CidTable *rt) \n";
        this->rt = rt;
    }
    IIpv4CidTable *getCidTable() const { return rt; }

    virtual bool isValid() const { return true; }

    /* operation: set */
    virtual void setIpaddr(Ipv4Address _ipaddr) { if (ipaddr != _ipaddr) { ipaddr = _ipaddr; changed(F_DESTINATION); } }
    virtual void setCid(ClientId _cid) { if (cid != _cid) { cid = _cid; changed(F_NEXTHOP); } }
    void setInterface(InterfaceEntry *_interfacePtr) override { if (interfacePtr != _interfacePtr) { interfacePtr = _interfacePtr; changed(F_IFACE); } }
    void setSourceType(SourceType _source) override { if (sourceType != _source) { sourceType = _source; changed(F_SOURCE); } }
    /* operation: get */
    const char* getSourceTypeAbbreviation() const;
    Ipv4Address getIpaddr() const { return ipaddr; }
    ClientId getCid() const override { return cid; }
    InterfaceEntry *getInterface() const override { return interfacePtr; }
    const char *getInterfaceName() const;
    SourceType getSourceType() const override { return sourceType; }
    unsigned int getAdminDist() const { return adminDist; }

    virtual ICidTable *getCidTableAsGeneric() const override;
    virtual void setIpaddr(const L3Address& ipaddr) override { setIpaddr(ipaddr.toIpv4()); }
    virtual void setCid(const L3Address& cid) override { setCid(cid.toClientId()); }
    virtual L3Address getIpaddrAsGeneric() const override { return getIpaddr(); }
    virtual ClientId getCidAsGeneric() const override { return getCid(); }
};

} // namespace inet

#endif // ifndef __INET_IPV4CID_H

