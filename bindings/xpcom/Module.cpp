#include <nsIGenericFactory.h>
#include "AudiereService.h"


NS_GENERIC_FACTORY_CONSTRUCTOR(AudiereService)


static nsModuleComponentInfo components[] = { {
  "XPCOM Audiere Component",
  AUDIERE_SERVICE_CID,
  AUDIERE_SERVICE_CONTRACTID,
  AudiereServiceConstructor,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
} };


NS_IMPL_NSGETMODULE(AudiereModule, components)
