// src/inet/networklayer/contract/ICidTable.h 

#ifndef __INET_ICIDTABLE_H
#define __INET_ICIDTABLE_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/ICid.h"

namespace inet {

/**
 * A C++ interface to abstract the functionality of a ClientId table, regardless of Ip address type.
 */
class INET_API ICidTable {
  public:
    virtual ~ICidTable() {};
    /**
     * Returns the total number of ClientIds.
     */
    virtual int getNumCids() const = 0;
    /**
     * Prints the ClientId table.
     */
    virtual void printCidTable() const = 0;
    /**
     * Returns the kth ClientId.
     */
    virtual ICid *getCid(int k) const = 0;
    /**
     * Adds a ClientId to the ClientId table. ClientIds are allowed to be modified
     * while in the cid table. (There is a notification mechanism that
     * allows cid table internals to be updated on a cid entry change.)
     */
    virtual void addCid(ICid *entry) = 0;
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const L3Address& address) const = 0;    //XXX should be find..., see next one
};

} // namespace inet

#endif // ifndef __INET_ICIDTABLE_H

