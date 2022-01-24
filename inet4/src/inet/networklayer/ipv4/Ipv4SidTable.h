// src/inet/networklayer/ipv4/Ipv4SidTable.h 

#ifndef __INET_IPV4SIDTABLE_H
#define __INET_IPV4SIDTABLE_H

#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/serviceid/ServiceId.h"
#include "inet/networklayer/ipv4/IIpv4SidTable.h"

namespace inet {

class IInterfaceTable;
class ISidTable;

/**
 * Represents the sid table. This object has one instance per host or router.
 */
class INET_API Ipv4SidTable : public cSimpleModule, public IIpv4SidTable,
                                protected cListener, public ILifecycle {
  protected:
    IInterfaceTable *ift = nullptr;    // cached pointer
    Ipv4Address sidId;
    bool isNodeUp = false;

    // sid cache: maps IP address to Sid
    typedef std::map<Ipv4Address, Ipv4Sid *> SidCache;  // ^-^OK^-^
    mutable SidCache sidCache;

  private:
    // The vectors storing Ipv4Sid.
    typedef std::vector<Ipv4Sid *> SidVector;
    SidVector sids;

  protected:
    // set sid Id
    virtual void configureSidId();

    // invalidates sid cache and local addresses cache
    virtual void invalidateCache();

    // helper for sorting ServiceId table, used by addSid()
    class SidLessThan {
        const Ipv4SidTable &c;
      public:
        SidLessThan(const Ipv4SidTable& c) : c(c) {}
        bool operator () (const Ipv4Sid *a, const Ipv4Sid *b) { return c.sidLessThan(a, b); }
    };
    bool sidLessThan(const Ipv4Sid *a, const Ipv4Sid *b) const;

    // helper functions:
    void internalAddSid(Ipv4Sid *entry);
    Ipv4Sid *internalRemoveSid(Ipv4Sid *entry);

  public:
    Ipv4SidTable() {}
    virtual ~Ipv4SidTable();

  protected:
    virtual void initialize(int stage) override;

  public:
    /**
     * For debugging
     */
    virtual void printSidTable() const override;

    /** @name Interfaces */
    //@{
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const Ipv4Address& address) const override;
    //@}

    /** @name ServiceId table manipulation */
    //@{
    /**
     * Returns the total number of sids.
     */
    virtual int getNumSids() const override { return sids.size(); }
    /**
     * Returns the kth sid.
     */
    virtual Ipv4Sid *getSid(int k) const override;
    /**
     * Adds a Ipv4Cid to the sid table. Ipv4Sid are allowed to be modified
     * while in the sid table. (There is a notification mechanism that
     * allows sid table internals to be updated on a sid entry change.)
     */
    virtual void addSid(Ipv4Sid *entry) override;
    /**
     * To be called from sid objects whenever a field changes. Used for
     * maintaining internal data structures and firing "sid table changed"
     * notifications.
     */
    virtual void ipv4SidChanged(Ipv4Sid *entry, int fieldCode) override;
    //@}

    /**
     * ILifecycle method
     */
    virtual bool handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback) override;
    virtual InterfaceEntry *getInterfaceByAddress(const L3Address& address) const override { return getInterfaceByAddress(address.toIpv4()); }

  private:
    virtual void addSid(ISid *entry) override { addSid(check_and_cast<Ipv4Sid *>(entry)); }
};

} // namespace inet

#endif // ifndef __INET_IPV4SIDTABLE_H




