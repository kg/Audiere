#include <nsMemory.h>
#include "AudioDevice.h"
#include "OutputStream.h"


AudioDevice::AudioDevice(audiere::AudioDevice* device) {
  NS_INIT_ISUPPORTS();
  m_device = device;
}


NS_IMPL_ISUPPORTS1(AudioDevice, adrIAudioDevice);


NS_IMETHODIMP
AudioDevice::CreateTone(double frequency, adrIOutputStream** rv) {
  if (!rv) {
    return NS_ERROR_NULL_POINTER;
  }

  // open the audio stream
  audiere::OutputStream* stream = m_device->openStream(
    audiere::CreateTone(frequency));
  if (stream) {
    *rv = new OutputStream(stream);
    NS_ADDREF(*rv);
    return NS_OK;
  } else {
    *rv = 0;
    return NS_ERROR_FAILURE;
  }
}


NS_IMETHODIMP
AudioDevice::OpenSound(
  const char* filename,
  PRBool streaming,
  adrIOutputStream** rv)
{
  if (!rv) {
    return NS_ERROR_NULL_POINTER;
  }

  // open the audio stream
  audiere::OutputStream* stream = audiere::OpenSound(
    m_device.get(), filename, streaming == PR_TRUE);
  if (stream) {
    *rv = new OutputStream(stream);
    NS_ADDREF(*rv);
    return NS_OK;
  } else {
    *rv = 0;
    return NS_ERROR_FAILURE;
  }
}
