// src/inet/networklayer/ipv4/IIpv4SidTable.h

#ifndef __INET_IIPV4SIDTABLE_H
#define __INET_IIPV4SIDTABLE_H

#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/networklayer/contract/ISidTable.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/serviceid/ServiceId.h"
#include "inet/networklayer/ipv4/Ipv4Sid.h"

namespace inet {

/**
 * A C++ interface to abstract the functionality of IIpv4SidTable.
 */
class INET_API IIpv4SidTable : public ISidTable {
  public:
    virtual ~IIpv4SidTable() {};

    /** @name Interfaces */
    //@{
    /**
     * Returns an interface given by its address. Returns nullptr if not found.
     */
    virtual InterfaceEntry *getInterfaceByAddress(const Ipv4Address& address) const = 0;
    using ISidTable::getInterfaceByAddress;
    //@}

    /** @name Sid table manipulation */
    //@{
    /**
     * Returns the kth sid.
     */
    virtual Ipv4Sid *getSid(int k) const override = 0;
    /**
     * Adds a Ipv4sid to the sid table. sids are allowed to be modified
     * while in the sid table. (There is a notification mechanism that
     * allows sid table internals to be updated on a sid entry change.)
     */
    virtual void addSid(Ipv4Sid *entry) = 0;
    using ISidTable::addSid;
    /**
     * To be called from sid objects whenever a field changes. Used for
     * maintaining internal data structures and firing "sid table changed"
     * notifications.
     */
    virtual void ipv4SidChanged(Ipv4Sid *entry, int fieldCode) = 0;
    //@}
};

} // namespace inet

#endif // ifndef __INET_IIPV4SIDTABLE_H

