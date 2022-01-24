// src/inet/networklayer/ipv4/Ipv4CidTable.h 

#ifndef __INET_IPV4CIDTABLE_H
#define __INET_IPV4CIDTABLE_H

#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/clientid/ClientId.h"
#include "inet/networklayer/ipv4/IIpv4CidTable.h"

namespace inet {

class IInterfaceTable;
class ICidTable;

/**
 * Represents the cid table. This object has one instance per host or router.
 */
class INET_API Ipv4CidTable : public cSimpleModule, public IIpv4CidTable,
                                protected cListener, public ILifecycle {
  protected:
    IInterfaceTable *ift = nullptr;    // cached pointer
    Ipv4Address cidId;
    bool isNodeUp = false;

    // cid cache: maps IP address to cid
    typedef std::map<Ipv4Address, Ipv4Cid *> CidCache;  // ^-^OK^-^
    mutable CidCache cidCache;

  private:
    // The vectors storing Ipv4Cid.
    typedef std::vector<Ipv4Cid *> CidVector;
    CidVector cids;

  protected:
    // set cid Id
    virtual void configureCidId();

    // invalidates cid cache and local addresses cache
    virtual void invalidateCache();

    // helper for sorting ServiceId table, used by addSid()
    class CidLessThan {
        const Ipv4CidTable &c;
      public:
        CidLessThan(const Ipv4CidTable& c) : c(c) {}
        bool operator () (const Ipv4Cid *a, const Ipv4Cid *b) { return c.cidLessThan(a, b); }
    };
    bool cidLessThan(const Ipv4Cid *a, const Ipv4Cid *b) const;

    // helper functions:
    void internalAddCid(Ipv4Cid *entry);
    Ipv4Cid *internalRemoveCid(Ipv4Cid *entry);

  public:
    Ipv4CidTable() {}
    virtual ~Ipv4CidTable();

  protected:
    virtual void initialize(int stage) override;

  public:
    /**
     * For debugging
     */
    virtual void printCidTable() const override;

    /** @name Interfaces */
    //@{
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const Ipv4Address& address) const override;
    //@}

    /** @name ClientId table manipulation */
    //@{
    /**
     * Returns the total number of cids.
     */
    virtual int getNumCids() const override { return cids.size(); }
    /**
     * Returns the kth cid.
     */
    virtual Ipv4Cid *getCid(int k) const override;
    /**
     * Adds a Ipv4Cid to the cid table. Ipv4Cid are allowed to be modified
     * while in the cid table. (There is a notification mechanism that
     * allows cid table internals to be updated on a cid entry change.)
     */
    virtual void addCid(Ipv4Cid *entry) override;
    /**
     * To be called from cid objects whenever a field changes. Used for
     * maintaining internal data structures and firing "cid table changed"
     * notifications.
     */
    virtual void ipv4CidChanged(Ipv4Cid *entry, int fieldCode) override;
    //@}

    /**
     * ILifecycle method
     */
    virtual bool handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback) override;
    virtual InterfaceEntry *getInterfaceByAddress(const L3Address& address) const override { return getInterfaceByAddress(address.toIpv4()); }

  private:
    virtual void addCid(ICid *entry) override { addCid(check_and_cast<Ipv4Cid *>(entry)); }
};

} // namespace inet

#endif // ifndef __INET_IPV4CIDTABLE_H




