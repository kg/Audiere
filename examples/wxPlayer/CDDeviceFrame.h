#ifndef ADR_CD_DEVICE_FRAME_H
#define ADR_CD_DEVICE_FRAME_H


#include <audiere.h>
#include "wx.h"


class CDDeviceFrame : public wxFrame {
public:
  CDDeviceFrame(audiere::CDDevicePtr device);

private:
  void OnDeviceNewDevice();
  void OnDeviceNewCDDevice();
  void OnDeviceNewMIDIDevice();
  void OnHelpAbout();

  void OnPlay();
  void OnStop();
  void OnPause();
  void OnResume();
  void OnOpenDoor();
  void OnCloseDoor();
  void OnCheckStatus();

  audiere::CDDevicePtr m_device;

  wxTextCtrl* m_track;

  DECLARE_EVENT_TABLE()
};


#endif
