// src/inet/networklayer/ipv4/IIpv4CidTable.h 

#ifndef __INET_IIPV4CIDTABLE_H
#define __INET_IIPV4CIDTABLE_H

#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ICidTable.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/clientid/ClientId.h"
#include "inet/networklayer/ipv4/Ipv4Cid.h"

namespace inet {

/**
 * A C++ interface to abstract the functionality of IIpv4CidTable.
 */
class INET_API IIpv4CidTable : public ICidTable {
  public:
    virtual ~IIpv4CidTable() {};

    /** @name Interfaces */
    //@{
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const Ipv4Address& address) const = 0;
    using ICidTable::getInterfaceByAddress;
    //@}

    /** @name Cid table manipulation */
    //@{
    /**
     * Returns the kth cid.
     */
    virtual Ipv4Cid *getCid(int k) const override = 0;
    /**
     * Adds a Ipv4cid to the cid table. cids are allowed to be modified
     * while in the cid table. (There is a notification mechanism that
     * allows cid table internals to be updated on a cid entry change.)
     */
    virtual void addCid(Ipv4Cid *entry) = 0;
    using ICidTable::addCid;
    /**
     * To be called from cid objects whenever a field changes. Used for
     * maintaining internal data structures and firing "cid table changed"
     * notifications.
     */
    virtual void ipv4CidChanged(Ipv4Cid *entry, int fieldCode) = 0;
    //@}
};

} // namespace inet

#endif // ifndef __INET_IIPV4CIDTABLE_H

