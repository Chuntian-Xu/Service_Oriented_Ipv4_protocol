// src/inet/networklayer/contract/ICidshoreTable.h

#ifndef __INET_ICIDSHORETABLE_H
#define __INET_ICIDSHORETABLE_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/ICidshore.h"

namespace inet {

/**
 * A C++ interface to abstract the functionality of a ClientId table, regardless of Ip address type.
 */
class INET_API ICidshoreTable {
  public:
    virtual ~ICidshoreTable() {};
    /**
     * Returns the total number of ClientIds.
     */
    virtual int getNumCidshores() const = 0;
    /**
     * Prints the ClientId table.
     */
    virtual void printCidshoreTable() const = 0;
    /**
     * Returns the kth ClientId.
     */
    virtual ICidshore *getCidshore(int k) const = 0;
    /**
     * Adds a ClientId to the ClientId table. ClientIds are allowed to be modified
     * while in the cidshore table. (There is a notification mechanism that
     * allows cidshore table internals to be updated on a cid entry change.)
     */
    virtual void addCidshore(ICidshore *entry) = 0;
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const L3Address& address) const = 0;    //XXX should be find..., see next one
};

} // namespace inet

#endif // ifndef __INET_ICIDSHORETABLE_H

