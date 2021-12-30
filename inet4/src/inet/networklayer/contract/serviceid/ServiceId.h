// src/inet/networklayer/contract/serviceid/ServiceId.h

#ifndef __INET_SERVICEID_H
#define __INET_SERVICEID_H

#include <iostream>
#include <string>

#include "inet/common/INETDefs.h"

namespace inet {

/**
 * Service Id.
 */
class INET_API ServiceId {
  protected:
    // Service Id is encoded in a single uint16 in host byte order
    // (e.g. DEC(6800) <=> BIN(00011010 10010000) <=> HEX(0x1A90) <=>"26.144")
    uint16 addr;

  protected:
    // Parses ServiceId into the given bytes, and returns true if syntax was OK.
    static bool parseServiceId(const char *text, unsigned char tobytes[]);

  public:
    /** ServiceId  category
     * ServiceId              Present Use
     * ---------------------------------------
     * 0000 0000 00000 000      UNSPECIFIED_MTC
     */
    enum SidCategory {
        UNSPECIFIED_MTC    // 0000 0000 0000 0000
    };

    /** @name Predefined Sid */
    //@{
    static const ServiceId UNSPECIFIED__MTC_Sid;    ///< 0000 0000 0000 0000
    //@}

    /** name Constructors, destructor */
    //@{
    /**
     * Default constructor, initializes to "0000 0000 0000 0000".
     */
    ServiceId() {addr=0;}
    /**
     * ServiceId as int
     */
    explicit ServiceId(uint16 sid) { addr = sid; }
    /**
     * Ipv4 address bytes: "i0.i1" format
     */
    ServiceId(int i0, int i1) { set(i0, i1); }
    /**
     * ServiceId given as text: "26.144"
     */
    explicit ServiceId(const char *text) { set(text); }
    /**
     * Copy constructor
     */
    ServiceId(const ServiceId& obj) { addr = obj.addr; }
    /**
     * destructor.
     */
    ~ServiceId() {}
    //@}

    /** name Setting ServiceId */
    //@{
    /**
     * ServiceId as uint16 in host byte order (e.g. "10.1" is 0x0A01)
     */
    void set(uint16 sid) { addr = sid; }

    /**
     * ServiceId bytes: "i0.i1" format
     */
    void set(int i0, int i1);

    /**
     * ServiceId given as text: "10.1"
     */
    void set(const char *t);
    //@}

    /**
     * Assignment
     */
    ServiceId& operator=(const ServiceId& obj) {addr = obj.addr; return *this;}

    /**
     * True if all two Sids bytes are zero.
     */
    bool isUnspecified() const {return addr==0;}

    /**
     * Returns true if the two ServiceId are equal
     */
    bool equals(const ServiceId& toCmp) const {return addr==toCmp.addr;}

    /**
     * Returns binary AND of the two ServiceId
     */
    ServiceId doAnd(const ServiceId& sid) const { return ServiceId(addr & sid.addr); }

    /**
     * Returns the string representation of the ServiceId (e.g. "10.1")
     * @param printUnspec: show "0.0" as "<unspec>" if true
     */
    std::string str(bool printUnspec = true) const;

    /**
     * Returns the ServiceId as an uint16 in host byte order (e.g. "10.1" is 0x0A01).
     */
    uint16 getInt() const {return addr;}

    /**
     * Get the ServiceId category.
     */
    SidCategory getSidCategory() const;

    /**
     * Returns equals.
     */
    bool operator==(const ServiceId& sid) const {return equals(sid);}
    /**
     * Returns !equals.
     */
    bool operator!=(const ServiceId& sid) const {return !equals(sid);}
    /**
     * Compares two Sid.
     */
    bool operator<(const ServiceId& sid) const {return getInt() < sid.getInt();}
    bool operator<=(const ServiceId& sid) const {return getInt() <= sid.getInt();}
    bool operator>(const ServiceId& sid) const {return getInt() > sid.getInt();}
    bool operator>=(const ServiceId& sid) const {return getInt() >= sid.getInt();}
};

inline std::ostream& operator<<(std::ostream& os, const ServiceId& sid) {
    return os << sid.str();
}

} // namespace inet

#endif // ifndef __INET_SERVICEID_H

