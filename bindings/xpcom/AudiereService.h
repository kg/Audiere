#ifndef XPCOM_AUDIERE_SERVICE_H
#define XPCOM_AUDIERE_SERVICE_H


#include "adrIAudiereService.h"


//  e4b993bd-bf4b-4555-8ddd-2576cce80bd0
#define AUDIERE_SERVICE_CID \
  { 0xe4b993bd, 0xbf4b, 0x4555, \
    { 0x8d, 0xdd, 0x25, 0x76, 0xcc, 0xe8, 0x0b, 0xd0 } }

#define AUDIERE_SERVICE_CONTRACTID "@aegisknight.org/audiere;1"


class AudiereService : public adrIAudiereService {
public:
  AudiereService();

  NS_DECL_ISUPPORTS
  NS_DECL_ADRIAUDIERESERVICE
};


#endif
