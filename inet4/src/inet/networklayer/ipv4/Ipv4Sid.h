// src/inet/networklayer/ipv4/Ipv4Sid.h

#ifndef __INET_IPV4SID_H
#define __INET_IPV4SID_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ISid.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/serviceid/ServiceId.h"

namespace inet {

class InterfaceEntry;
class IIpv4SidTable;

/**
 * Ipv4 sid in IIpv4SidTable.
 */
class INET_API Ipv4Sid : public cObject, public ISid {
  private:
    IIpv4SidTable *rt;
    InterfaceEntry *interfacePtr;    ///< interface
    SourceType sourceType;    ///< manual, routing prot, etc.
    unsigned int adminDist;    ///< Cisco like administrative distance
    Ipv4Address ipaddr;
    ServiceId sid;

  private:
    // copying not supported: following are private and also left undefined
    Ipv4Sid(const Ipv4Sid& obj);
    Ipv4Sid& operator=(const Ipv4Sid& obj);

  protected:
    void changed(int fieldCode);

  public:
    Ipv4Sid():rt(nullptr),interfacePtr(nullptr),sourceType(MANUAL),adminDist(dUnknown),ipaddr(),sid(){}
    virtual ~Ipv4Sid();
    virtual std::string str() const override;
    virtual std::string detailedInfo() const OMNETPP5_CODE(override);

    bool operator==(const Ipv4Sid& sid) const { return equals(sid); }
    bool operator!=(const Ipv4Sid& sid) const { return !equals(sid); }
    bool equals(const Ipv4Sid& sid) const;

    // ^-^
    /** To be called by the sid table when this sid is added or removed from it */
    virtual void setSidTable(IIpv4SidTable *rt) {
//        EV << " !!! --> Ipv4Sid.h::setSidTable(IIpv4SidTable *rt) \n";
        this->rt = rt;
    }
    IIpv4SidTable *getSidTable() const { return rt; }

    virtual bool isValid() const { return true; }

    /* operation: set */
    virtual void setIpaddr(Ipv4Address _ipaddr) { if (ipaddr != _ipaddr) { ipaddr = _ipaddr; changed(F_DESTINATION); } }
    virtual void setSid(ServiceId _sid) { if (sid != _sid) { sid = _sid; changed(F_NEXTHOP); } }
    void setInterface(InterfaceEntry *_interfacePtr) override { if (interfacePtr != _interfacePtr) { interfacePtr = _interfacePtr; changed(F_IFACE); } }
    void setSourceType(SourceType _source) override { if (sourceType != _source) { sourceType = _source; changed(F_SOURCE); } }
    /* operation: get */
    const char* getSourceTypeAbbreviation() const;
    Ipv4Address getIpaddr() const { return ipaddr; }
    ServiceId getSid() const override { return sid; }
    InterfaceEntry *getInterface() const override { return interfacePtr; }
    const char *getInterfaceName() const;
    SourceType getSourceType() const override { return sourceType; }
    unsigned int getAdminDist() const { return adminDist; }

    virtual ISidTable *getSidTableAsGeneric() const override;
    virtual void setIpaddr(const L3Address& ipaddr) override { setIpaddr(ipaddr.toIpv4()); }
    virtual void setSid(const L3Address& sid) override { setSid(sid.toServiceId()); }
    virtual L3Address getIpaddrAsGeneric() const override { return getIpaddr(); }
    virtual ServiceId getSidAsGeneric() const override { return getSid(); }
};

} // namespace inet

#endif // ifndef __INET_IPV4SID_H

