// src/inet/networklayer/contract/ISidTable.h 

#ifndef __INET_ISIDTABLE_H
#define __INET_ISIDTABLE_H

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/ISid.h"

namespace inet {

/**
 * A C++ interface to abstract the functionality of a ServiceId table, regardless of Ip address type.
 */
class INET_API ISidTable {
  public:
    virtual ~ISidTable() {};
    /**
     * Returns the total number of ServiceIds.
     */
    virtual int getNumSids() const = 0;
    /**
     * Prints the ServiceId table.
     */
    virtual void printSidTable() const = 0;
    /**
     * Returns the kth ServiceId.
     */
    virtual ISid *getSid(int k) const = 0;
    /**
     * Adds a ServiceId to the ServiceId table. ServiceIds are allowed to be modified
     * while in the sid table. (There is a notification mechanism that
     * allows sid table internals to be updated on a sid entry change.)
     */
    virtual void addSid(ISid *entry) = 0;
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const L3Address& address) const = 0;    //XXX should be find..., see next one
};

} // namespace inet

#endif // ifndef __INET_ISIDTABLE_H

