// src/inet/networklayer/ipv4/IIpv4CidshoreTable.h

#ifndef __INET_IIPV4CIDSHORETABLE_H
#define __INET_IIPV4CIDSHORETABLE_H

#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ICidshoreTable.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/networklayer/ipv4/Ipv4Cidshore.h"

namespace inet {

/**
 * A C++ interface to abstract the functionality of IIpv4CidshoreTable.
 */
class INET_API IIpv4CidshoreTable : public ICidshoreTable {
  public:
    virtual ~IIpv4CidshoreTable() {};

    /** @name Interfaces */
    //@{
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const Ipv4Address& address) const = 0;
    using ICidshoreTable::getInterfaceByAddress;
    //@}

    /** @name Cidshore table manipulation */
    //@{
    /**
     * Returns the kth cidshore.
     */
    virtual Ipv4Cidshore *getCidshore(int k) const override = 0;
    /**
     * Adds a Ipv4cidshore to the cidshore table. cidshores are allowed to be modified
     * while in the cidshore table. (There is a notification mechanism that
     * allows cidshore table internals to be updated on a cidshore entry change.)
     */
    virtual void addCidshore(Ipv4Cidshore *entry) = 0;
    using ICidshoreTable::addCidshore;
    /**
     * To be called from cid objects whenever a field changes. Used for
     * maintaining internal data structures and firing "cid table changed"
     * notifications.
     */
    virtual void ipv4CidshoreChanged(Ipv4Cidshore *entry, int fieldCode) = 0;
    //@}
};

} // namespace inet

#endif // ifndef __INET_IIPV4CIDSHORETABLE_H

