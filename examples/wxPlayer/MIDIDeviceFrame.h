#ifndef ADR_MIDI_DEVICE_FRAME_H
#define ADR_MIDI_DEVICE_FRAME_H


#include <audiere.h>
#include "wx.h"


class MIDIDeviceFrame : public wxMDIParentFrame {
public:
  MIDIDeviceFrame(audiere::MIDIDevicePtr device);

  void OnDeviceNewDevice();
  void OnDeviceNewCDDevice();
  void OnDeviceNewMIDIDevice();
  void OnDeviceOpenSong();
  void OnDeviceCloseCurrentWindow();
  void OnDeviceClose();
  void OnHelpAbout();

private:
  audiere::MIDIDevicePtr m_device;

  DECLARE_EVENT_TABLE()
};


#endif
