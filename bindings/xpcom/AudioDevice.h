#ifndef MOZ_AUDIO_CONTEXT_H
#define MOZ_AUDIO_CONTEXT_H


#include <audiere.h>
#include "adrIAudioDevice.h"


class AudioDevice : public adrIAudioDevice {
public:
  AudioDevice(audiere::AudioDevice* device);

  NS_DECL_ISUPPORTS
  NS_DECL_ADRIAUDIODEVICE

private:
  audiere::RefPtr<audiere::AudioDevice> m_device;
};


#endif
