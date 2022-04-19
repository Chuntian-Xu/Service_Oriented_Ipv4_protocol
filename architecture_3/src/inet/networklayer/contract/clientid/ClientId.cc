// src/inet/networklayer/contract/clientid/ClientId.cc
 
#include "inet/networklayer/contract/clientid/ClientId.h"
 
namespace inet {

static const int CLIENTID_STRING_SIZE = 20;

// predefined Cid
const ClientId ClientId::UNSPECIFIED_MTC_Cid;

bool ClientId::parseClientId(const char *text, unsigned char tobytes[]) {
    if (!text) return false;

    if (!strcmp(text, "<unspec>")) {
        tobytes[0] = 0;
        return true;
    }

    const char *s = text;
    int i = 0;
        if (*s < '0' || *s > '9') return false; // missing number
        // read and store number
        int num = 0;
        while (*s >= '0' && *s <= '9') num = 10 * num + (*s++ - '0');
        if (num > 255) return false; // number too big
        tobytes[i] = (unsigned char)num;
    return i == 0;    // must have all 1 numbers
}

void ClientId::set(int i0) {
    addr = (i0 << 8);
}

void ClientId::set(const char *text) {
    unsigned char buf[1];
    if (!text) throw cRuntimeError("ClientId string is nullptr");

    bool ok = parseClientId(text, buf);
    if (!ok) throw cRuntimeError("Invalid ClientId string `%s'", text);

    set(buf[0]);
}

ClientId::CidCategory ClientId::getCidCategory() const {
    if (isUnspecified()) return UNSPECIFIED_MTC_CLIENT; // 0000 0000
    return UNSPECIFIED_MTC_CLIENT;
}

std::string ClientId::str(bool printUnspec    /* = true */) const {
    if (printUnspec && isUnspecified()) return std::string("<unspec>");
    char buf[CLIENTID_STRING_SIZE];
    sprintf(buf, "%u", addr & 255);
    return std::string(buf);
}

} // namespace inet

