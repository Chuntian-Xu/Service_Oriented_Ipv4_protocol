// src/inet/networklayer/ipv4/Ipv4CidshoreTable.h

#ifndef __INET_IPV4CIDSHORETABLE_H
#define __INET_IPV4CIDSHORETABLE_H

#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/networklayer/ipv4/IIpv4CidshoreTable.h"

namespace inet {

class IInterfaceTable;
class ICidshoreTable;

/**
 * Represents the cidshore table. This object has one instance per host or router.
 */
class INET_API Ipv4CidshoreTable : public cSimpleModule, public IIpv4CidshoreTable,
                                protected cListener, public ILifecycle {
  protected:
    IInterfaceTable *ift = nullptr;    // cached pointer
    Ipv4Address cidshoreId;
    bool isNodeUp = false;

    // cid cache: maps IP address to cid
    typedef std::map<Ipv4Address, Ipv4Cidshore *> CidshoreCache;  // ^-^OK^-^
    mutable CidshoreCache cidshoreCache;

  private:
    // The vectors storing Ipv4Cidshore.
    typedef std::vector<Ipv4Cidshore *> CidshoreVector;
    CidshoreVector cidshores;

  protected:
    // set cidshore_Id
    virtual void configureCidshoreId();

    // invalidates cidshore cache and local addresses cache
    virtual void invalidateCache();

    // helper for sorting ServiceId table, used by addSidshore()
    class CidshoreLessThan {
        const Ipv4CidshoreTable &c;
      public:
        CidshoreLessThan(const Ipv4CidshoreTable& c) : c(c) {}
        bool operator () (const Ipv4Cidshore *a, const Ipv4Cidshore *b) { return c.cidshoreLessThan(a, b); }
    };
    bool cidshoreLessThan(const Ipv4Cidshore *a, const Ipv4Cidshore *b) const;

    // helper functions:
    void internalAddCidshore(Ipv4Cidshore *entry);
    Ipv4Cidshore *internalRemoveCidshore(Ipv4Cidshore *entry);

  public:
    Ipv4CidshoreTable() {}
    virtual ~Ipv4CidshoreTable();

  protected:
    virtual void initialize(int stage) override;

  public:
    /**
     * For debugging
     */
    virtual void printCidshoreTable() const override;

    /** @name Interfaces */
    //@{
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const Ipv4Address& address) const override;
    //@}

    /** @name ClientId_shore_table manipulation */
    //@{
    /**
     * Returns the total number of cidshores.
     */
    virtual int getNumCidshores() const override { return cidshores.size(); }
    /**
     * Returns the kth cidshore.
     */
    virtual Ipv4Cidshore *getCidshore(int k) const override;
    /**
     * Adds a Ipv4Cid to the cidshore table. Ipv4Cidshore are allowed to be modified
     * while in the cidshore table. (There is a notification mechanism that
     * allows cidshore table internals to be updated on a cidshore entry change.)
     */
    virtual void addCidshore(Ipv4Cidshore *entry) override;
    /**
     * To be called from cidshore objects whenever a field changes. Used for
     * maintaining internal data structures and firing "cidshore table changed"
     * notifications.
     */
    virtual void ipv4CidshoreChanged(Ipv4Cidshore *entry, int fieldCode) override;
    //@}

    /**
     * ILifecycle method
     */
    virtual bool handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback) override;
    virtual InterfaceEntry *getInterfaceByAddress(const L3Address& address) const override { return getInterfaceByAddress(address.toIpv4()); }

  private:
    virtual void addCidshore(ICidshore *entry) override { addCidshore(check_and_cast<Ipv4Cidshore *>(entry)); }
};

} // namespace inet

#endif // ifndef __INET_IPV4SHORECIDTABLE_H




