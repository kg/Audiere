#include <audiere.h>
#include "wxPlayer.h"
#include "DeviceFrame.h"


IMPLEMENT_APP(wxPlayer)


bool wxPlayer::OnInit() {
  bool result = (TryDevice("autodetect") || TryDevice("null"));
  if (!result) {
    wxMessageBox("Could not open output device", "wxPlayer");
  }
  return result;
}


bool wxPlayer::TryDevice(const std::string& name, const std::string& parameters) {
  if (audiere::AudioDevicePtr device = audiere::OpenDevice(name.c_str(), parameters.c_str())) {
    DeviceFrame* frame = new DeviceFrame(device.get());
    frame->Show(true);
    return true;
  } else {
    return false;
  }
}
