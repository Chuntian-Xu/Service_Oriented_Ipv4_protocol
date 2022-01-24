// src/inet/networklayer/ipv4/Ipv4NetworkLayerMobileStation.h
 
#ifndef __INET_IPV4NETWORKLAYERMOBILESTATION_H
#define __INET_IPV4NETWORKLAYERMOBILESTATION_H

#include "inet/common/INETDefs.h"

namespace inet {

class INET_API Ipv4NetworkLayerMobileStation : public cModule {
  protected:
    virtual void refreshDisplay() const override;
    virtual void updateDisplayString() const;
};

} // namespace inet

#endif // ifndef __INET_IPV4NETWORKLAYERMOBILESTATION_H

