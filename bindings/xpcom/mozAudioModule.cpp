#include "nsIGenericFactory.h"
#include "mozAudioService.h"


NS_GENERIC_FACTORY_CONSTRUCTOR(mozAudioService)


static nsModuleComponentInfo components[] =
{ {

  "Mozilla Audio Component",
  MOZ_AUDIO_SERVICE_CID,
  MOZ_AUDIO_SERVICE_CONTRACTID,
  mozAudioServiceConstructor,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  
} };


NS_IMPL_NSGETMODULE(mozAudioModule, components)
