#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <audiere.h>
#include "CDDeviceDialog.h"
#include "CDDeviceFrame.h"
#include "DeviceFrame.h"
#include "NewDeviceDialog.h"
#include "wxPlayer.h"


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
    (new DeviceFrame(device.get()))->Show(true);
    return true;
  } else {
    return false;
  }
}


void wxPlayer::OnNewDevice(wxWindow* parent) {
  NewDeviceDialog dialog(parent);
  if (dialog.ShowModal() == wxID_OK) {
    bool result = TryDevice(
      dialog.getDevice().c_str(),
      dialog.getParameters().c_str());
    if (!result) {
      wxMessageBox("Error opening new device", "New Device",
                   wxOK | wxICON_ERROR, parent);
    }
  }
}


void wxPlayer::OnNewCDDevice(wxWindow* parent) {
  CDDeviceDialog dialog(parent);
  if (dialog.ShowModal() == wxID_OK) {
    audiere::CDDevice* device = audiere::OpenCDDevice(dialog.getName().c_str());
    if (!device) {
      wxMessageBox("Error opening CD device", "New CD Device",
                   wxOK | wxICON_ERROR, parent);
      return;
    }

    (new CDDeviceFrame(device))->Show(true);
  }
}


void wxPlayer::ShowAboutDialog(wxWindow* parent) {
  wxString message =
    "wxPlayer\n"
    "Copyright (c) Chad Austin 2002-2004\n\n"
    "Audiere Version: ";
  message += audiere::GetVersion();
  wxMessageBox(message, "About wxPlayer", wxOK | wxCENTRE, parent);
}
