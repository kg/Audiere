#ifndef ADR_DEVICE_FRAME_H
#define ADR_DEVICE_FRAME_H


#include <audiere.h>
#include "wx.h"


class DeviceFrame : public wxMDIParentFrame {
public:
  DeviceFrame(audiere::AudioDevicePtr device);
  
  void OnDeviceNewDevice();
  void OnDeviceNewCDDevice();
  void OnDeviceNewMIDIDevice();
  wxString GetSoundFile();
  void OnDeviceOpenStream();
  void OnDeviceOpenSound();
  void OnDeviceCreateTone();
  void OnDeviceCreateSquareWave();
  void OnDeviceCreateWhiteNoise();
  void OnDeviceCreatePinkNoise();
  void OnDeviceOpenSingleEffect();
  void OnDeviceOpenMultipleEffect();
  void DoOpenEffect(audiere::SoundEffectType type, wxString typestring);
  void OnDeviceCloseCurrentWindow();
  void OnDeviceClose();
  void OnHelpAbout();

private:
  audiere::AudioDevicePtr m_device;

  DECLARE_EVENT_TABLE()
};


#endif
