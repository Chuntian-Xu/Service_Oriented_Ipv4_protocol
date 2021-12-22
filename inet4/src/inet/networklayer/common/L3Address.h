// src/inet/networklayer/common/L3Address.h

#ifndef __INET_L3ADDRESS_H
#define __INET_L3ADDRESS_H

#include "inet/common/INETDefs.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/networklayer/common/ModuleIdAddress.h"
#include "inet/networklayer/common/ModulePathAddress.h"
#include "inet/networklayer/contract/clns/ClnsAddress.h"
#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/contract/ipv6/Ipv6Address.h"

#include "inet/networklayer/contract/serviceid/ServiceId.h" // new added

namespace inet {

class IL3AddressType;

/**
 * This class provides the generic interface for network addresses. For efficiency reasons the
 * implementation uses a 128 bits integer to represent all kinds of network addresses. The
 * different address implementations should not subclass this class, they should rather provide
 * conversions to and from this class.
 *
 * @see Ipv4Address, Ipv6Address, CLNSAddress, MacAddress, ModulePathAddress, ModuleIdAddress
 */
class INET_API L3Address
{
  public:
    enum AddressType {
        NONE,
        IPv4,
        SERVICEID, // new added
        IPv6,
        CLNS,
        MAC,
        MODULEPATH,
        MODULEID
    };

  private:
    uint64 hi;
    uint64 lo;

  private:
    uint64 get(AddressType type) const;
    void set(AddressType type, uint64 lo);

  public:
    L3Address() { set(NONE, 0); }
    L3Address(const L3Address& other) : hi(other.hi), lo(other.lo) { }
    explicit L3Address(const char *str) { tryParse(str); }
    L3Address(const Ipv4Address& addr) { set(addr); }

    L3Address(const ServiceId& addr) { set(addr); } // new added

    L3Address(const Ipv6Address& addr) { set(addr); }
    L3Address(const MacAddress& addr) { set(addr); }
    L3Address(const ModuleIdAddress& addr) { set(addr); }
    L3Address(const ModulePathAddress& addr) { set(addr); }
    L3Address(const ClnsAddress& addr) { set(addr); }

    void set(const Ipv4Address& addr) {
//        EV_INFO<<"!!! --> void set(const Ipv4Address& addr)\n";
        set(IPv4, addr.getInt());
        }
    void set(const ServiceId& addr) {
//        EV_INFO<<"!!! --> void set(const ServiceId& addr)\n";
        set(SERVICEID, addr.getInt());
    } // new added
    void set(const Ipv6Address& addr);
    void set(const MacAddress& addr) { set(MAC, addr.getInt()); }
    void set(const ModuleIdAddress& addr) { set(MODULEID, addr.getId()); }
    void set(const ModulePathAddress& addr) { set(MODULEPATH, addr.getId()); }
    void set(const ClnsAddress& addr);
    void reset() { set(NONE, 0); }

    Ipv4Address toIpv4() const { return getType() == NONE ? Ipv4Address() : Ipv4Address(get(IPv4)); }
    ServiceId toServiceId() const { return getType() == NONE ? ServiceId() : ServiceId(get(SERVICEID)); } // new added
    Ipv6Address toIpv6() const { return getType() == NONE ? Ipv6Address() : Ipv6Address(hi, get(IPv6)); }
    MacAddress toMac() const { return getType() == NONE ? MacAddress() : MacAddress(get(MAC)); }
    ModuleIdAddress toModuleId() const { return getType() == NONE ? ModuleIdAddress() : ModuleIdAddress(get(MODULEID)); }
    ModulePathAddress toModulePath() const { return getType() == NONE ? ModulePathAddress() : ModulePathAddress(get(MODULEPATH)); }
    ClnsAddress toClns() const;

    std::string str() const;
    AddressType getType() const;
    IL3AddressType *getAddressType() const;

    /**
     * Get the first prefixLength bits of the address, with the rest set to zero.
     */
    L3Address getPrefix(int prefixLength) const;

    bool tryParse(const char *addr);

    bool isUnspecified() const;
    bool isUnicast() const;
    bool isMulticast() const;
    bool isBroadcast() const;
    bool isLinkLocal() const;

    L3Address& operator=(const L3Address& other) { hi = other.hi; lo = other.lo; return *this; }

    bool operator<(const L3Address& other) const;
    bool operator>(const L3Address& other) const { return other < *this; };
    bool operator==(const L3Address& other) const;
    bool operator!=(const L3Address& other) const;

    bool matches(const L3Address& other, int prefixLength) const;

    static const char *getTypeName(AddressType t);

    inline void doParsimPacking(cCommBuffer *buf) const
    {
        buf->pack(hi);
        buf->pack(lo);
    }

    inline void doParsimUnpacking(cCommBuffer *buf)
    {
        buf->unpack(hi);
        buf->unpack(lo);
    }
};

inline std::ostream& operator<<(std::ostream& os, const L3Address& address)
{
    return os << address.str();
}

inline void doParsimPacking(cCommBuffer *buf, const L3Address& addr)
{
    addr.doParsimPacking(buf);
}

inline void doParsimUnpacking(cCommBuffer *buf, L3Address& addr)
{
    addr.doParsimUnpacking(buf);
}

} // namespace inet

#endif // ifndef __INET_ADDRESS_H

