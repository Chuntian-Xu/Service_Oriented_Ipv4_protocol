// src/inet/networklayer/contract/clientid/ClientId.h
 
#ifndef __INET_CLIENTID_H
#define __INET_CLIENTID_H

#include <iostream>
#include <string>

#include "inet/common/INETDefs.h"

namespace inet {

/**
 * Client Id.
 */
class INET_API ClientId {
  protected:
    // Client Id is encoded in a single uint8 in host byte order
    // (e.g. DEC(23) <=> BIN(0001 0111) <=> HEX(0x17) <=>"23")
    uint8 addr;

  protected:
    // Parses ClientId into the given bytes, and returns true if syntax was OK.
    static bool parseClientId(const char *text, unsigned char tobytes[]);

  public:
    /** ClientId  category
     * ClientId              Present Use
     * ---------------------------------------
     * 0000 0000      UNSPECIFIED_MTC_CLIENT
     */
    enum CidCategory {
        UNSPECIFIED_MTC_CLIENT    // 0000 0000
    };

    /** @name Predefined cid */
    //@{
    static const ClientId UNSPECIFIED_MTC_Cid;    ///< 0000 0000
    //@}

    /** name Constructors, destructor */
    //@{
    /**
     * Default constructor, initializes to "0000 0000".
     */
    ClientId() {addr=0;}
    /**
     * ClientId as int
     */
    explicit ClientId(uint8 cid) { addr = cid; }
    /**
     * Ipv4 address bytes: "i0" format
     */
    ClientId(int i0) { set(i0); }
    /**
     * ClientId given as text: "26"
     */
    explicit ClientId(const char *text) { set(text); }
    /**
     * Copy constructor
     */
    ClientId(const ClientId& obj) { addr = obj.addr; }
    /**
     * destructor.
     */
    ~ClientId() {}
    //@}

    /** name Setting ClientId */
    //@{
    /**
     * ClientId as uint8 in host byte order (e.g. "10" is 0x0A)
     */
    void set(uint8 cid) { addr = cid; }

    /**
     * ClientId bytes: "i0" format
     */
    void set(int i0);

    /**
     * ClientId given as text: "10"
     */
    void set(const char *t);
    //@}

    /**
     * Assignment
     */
    ClientId& operator=(const ClientId& obj) {addr = obj.addr; return *this;}

    /**
     * True if all two cids bytes are zero.
     */
    bool isUnspecified() const {return addr==0;}

    /**
     * Returns true if the two ClientId are equal
     */
    bool equals(const ClientId& toCmp) const {return addr==toCmp.addr;}

    /**
     * Returns binary AND of the two ClientId
     */
    ClientId doAnd(const ClientId& cid) const { return ClientId(addr & cid.addr); }

    /**
     * Returns the string representation of the ClientId (e.g. "10.1")
     * @param printUnspec: show "0" as "<unspec>" if true
     */
    std::string str(bool printUnspec = true) const;

    /**
     * Returns the ClientId as an uint8 in host byte order (e.g. "10" is 0x0A).
     */
    uint8 getInt() const {return addr;}

    /**
     * Get the ClientId category.
     */
    CidCategory getCidCategory() const;

    /**
     * Returns equals.
     */
    bool operator==(const ClientId& cid) const {return equals(cid);}
    /**
     * Returns !equals.
     */
    bool operator!=(const ClientId& cid) const {return !equals(cid);}
    /**
     * Compares two cid.
     */
    bool operator<(const ClientId& cid) const {return getInt() < cid.getInt();}
    bool operator<=(const ClientId& cid) const {return getInt() <= cid.getInt();}
    bool operator>(const ClientId& cid) const {return getInt() > cid.getInt();}
    bool operator>=(const ClientId& cid) const {return getInt() >= cid.getInt();}
};

inline std::ostream& operator<<(std::ostream& os, const ClientId& cid) {
    return os << cid.str();
}

} // namespace inet

#endif // ifndef __INET_CLIENTID_H

