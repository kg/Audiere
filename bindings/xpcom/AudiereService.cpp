#include <audiere.h>
#include <nsMemory.h>
#include "AudiereService.h"
#include "AudioDevice.h"


AudiereService::AudiereService() {
  NS_INIT_ISUPPORTS();
}


NS_IMPL_ISUPPORTS1(AudiereService, adrIAudiereService);


NS_IMETHODIMP
AudiereService::GetVersion(char** rv) {
  const char* version = audiere::GetVersion();
  *rv = (char*)nsMemory::Clone(version, strlen(version) + 1);
  return NS_OK;
}


NS_IMETHODIMP
AudiereService::OpenDevice(
  const char* name,
  const char* parameters,
  adrIAudioDevice** rv)
{
  if (!rv) {
    return NS_ERROR_NULL_POINTER;
  }

  audiere::AudioDevice* device = audiere::OpenDevice(name, parameters);
  if (device) {
    *rv = new AudioDevice(device);
    NS_ADDREF(*rv);
    return NS_OK;
  } else {
    *rv = 0;
    return NS_ERROR_FAILURE;
  }
}
