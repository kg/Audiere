#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <set>
#include <string>
#include <vector>
#include "Commands.h"
#include "DeviceFrame.h"
#include "NewDeviceDialog.h"
#include "SoundEffectFrame.h"
#include "StreamFrame.h"
#include "wxPlayer.h"


template<typename T>
std::string Join(
  T cont,
  const std::string& joiner,
  const std::string& prefix = "")
{
  std::string result;

  typename T::iterator i = cont.begin();
  for (;;) {
    result += prefix + *i++;
    if (i == cont.end()) {
      break;
    } else {
      result += joiner;
    }
  }

  return result;
}


BEGIN_EVENT_TABLE(DeviceFrame, wxMDIParentFrame)
  EVT_MENU(DEVICE_NEW_DEVICE,           DeviceFrame::OnDeviceNewDevice)
  EVT_MENU(DEVICE_OPEN_STREAM,          DeviceFrame::OnDeviceOpenStream)
  EVT_MENU(DEVICE_OPEN_SOUND,           DeviceFrame::OnDeviceOpenSound)
  EVT_MENU(DEVICE_CREATE_TONE,          DeviceFrame::OnDeviceCreateTone)
  EVT_MENU(DEVICE_CREATE_SQUARE_WAVE,   DeviceFrame::OnDeviceCreateSquareWave)
  EVT_MENU(DEVICE_CREATE_WHITE_NOISE,   DeviceFrame::OnDeviceCreateWhiteNoise)
  EVT_MENU(DEVICE_CREATE_PINK_NOISE,    DeviceFrame::OnDeviceCreatePinkNoise)
  EVT_MENU(DEVICE_OPEN_SINGLE_EFFECT,   DeviceFrame::OnDeviceOpenSingleEffect)
  EVT_MENU(DEVICE_OPEN_MULTIPLE_EFFECT, DeviceFrame::OnDeviceOpenMultipleEffect)
  EVT_MENU(DEVICE_CLOSE_WINDOW,         DeviceFrame::OnDeviceCloseCurrentWindow)
  EVT_MENU(DEVICE_CLOSE,                DeviceFrame::OnDeviceClose)
  EVT_MENU(HELP_ABOUT,                  DeviceFrame::OnHelpAbout)
END_EVENT_TABLE()


DeviceFrame::DeviceFrame(audiere::AudioDevice* device)
: wxMDIParentFrame(0, -1, "Device Window - " + wxString(device->getName()))
{
  m_device = device;

  wxMenu* fileMenu = new wxMenu();
  fileMenu->Append(DEVICE_NEW_DEVICE,           "&New Device...");
  fileMenu->AppendSeparator();
  fileMenu->Append(DEVICE_OPEN_STREAM,          "&Open Stream...");
  fileMenu->Append(DEVICE_OPEN_SOUND,           "Open &Sound...");
  fileMenu->AppendSeparator();
  fileMenu->Append(DEVICE_CREATE_TONE,          "Create &Tone...");
  fileMenu->Append(DEVICE_CREATE_SQUARE_WAVE,   "Create S&quare Wave...");
  fileMenu->Append(DEVICE_CREATE_WHITE_NOISE,   "Create &White Noise");
  fileMenu->Append(DEVICE_CREATE_PINK_NOISE,    "Create &Pink Noise");
  fileMenu->AppendSeparator();
  fileMenu->Append(DEVICE_OPEN_SINGLE_EFFECT,   "Open &Effect (Single)...");
  fileMenu->Append(DEVICE_OPEN_MULTIPLE_EFFECT, "Open Effect (&Multiple)...");
  fileMenu->AppendSeparator();
  fileMenu->Append(DEVICE_CLOSE_WINDOW,         "Close C&urrent Window");
  fileMenu->Append(DEVICE_CLOSE,                "&Close Device");

  wxMenu* helpMenu = new wxMenu();
  helpMenu->Append(HELP_ABOUT, "&About...");

  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append(fileMenu, "&Device");
  menuBar->Append(helpMenu, "&Help");
  SetMenuBar(menuBar);

  SetFocus();
}


void DeviceFrame::OnDeviceNewDevice() {
  NewDeviceDialog dialog(this);
  if (dialog.ShowModal() == wxID_OK) {
    bool result = wxGetApp().TryDevice(
      dialog.getDevice().c_str(),
      dialog.getParameters().c_str());
    if (!result) {
      wxMessageBox("Error opening new device", "New Device",
                   wxOK | wxICON_ERROR, this);
    }
  }
}


wxString DeviceFrame::GetSoundFile() {
  std::vector<audiere::FileFormatDesc> formats;
  audiere::GetSupportedFileFormats(formats);

  // combine all of the supported extensions into one collection
  std::set<std::string> all_extensions;
  for (unsigned i = 0; i < formats.size(); ++i) {
    for (unsigned j = 0; j < formats[i].extensions.size(); ++j) {
      all_extensions.insert("*." + formats[i].extensions[j]);
    }
  }

  // build a string of wildcards for wxWindows
  std::string wildcards;
  wildcards = "Sound Files (" + Join(all_extensions, ",") + ")|";
  wildcards += Join(all_extensions, ";") + "|";

  for (unsigned i = 0; i < formats.size(); ++i) {
    audiere::FileFormatDesc& ffd = formats[i];
    wildcards += ffd.description + " ";
    wildcards += "(" + Join(ffd.extensions, ",", "*.") + ")|";
    wildcards += Join(ffd.extensions, ";", "*.") + "|";
  }

  wildcards += "All Files (*.*)|*.*";

  return wxFileSelector(
    "Select a sound file", "", "", "",
    wildcards.c_str(), wxOPEN, this);
}


void DeviceFrame::OnDeviceOpenStream() {
  wxString filename(GetSoundFile());
  if (filename.empty()) {
    return;
  }

  audiere::OutputStream* stream = audiere::OpenSound(m_device, filename, true);
  if (!stream) {
    wxMessageBox(
      "Could not open stream: " + filename,
      "Open Stream", wxOK | wxCENTRE, this);
    return;
  }

  // get the basename of the path for the window title
  wxString basename = wxFileNameFromPath(filename);
  new StreamFrame(this, "Stream: " + basename, stream);
}


void DeviceFrame::OnDeviceOpenSound() {
  wxString filename(GetSoundFile());
  if (filename.empty()) {
    return;
  }

  audiere::OutputStream* stream = audiere::OpenSound(m_device, filename);
  if (!stream) {
    wxMessageBox(
      "Could not open sound: " + filename,
      "Open Stream", wxOK | wxCENTRE, this);
    return;
  }

  // get the basename of the path for the window title
  wxString basename = wxFileNameFromPath(filename);
  new StreamFrame(this, "Sound: " + basename, stream);
}


void DeviceFrame::OnDeviceCreateTone() {
  int frequency = wxGetNumberFromUser(
    "Value must be between 1 and 30000.", "Enter frequency in Hz",
    "Create Tone", 256, 1, 30000, this);
  if (frequency != -1) {
    audiere::OutputStream* stream = m_device->openStream(audiere::CreateTone(frequency));
    if (!stream) {
      wxMessageBox(
        "Could not open output stream",
        "Create Tone", wxOK | wxCENTRE, this);
    } else {
      wxString title;
      title.sprintf("Tone: %d Hz", frequency);
      new StreamFrame(this, title, stream);
    }
  }
}


void DeviceFrame::OnDeviceCreateSquareWave() {
  int frequency = wxGetNumberFromUser(
    "Value must be between 1 and 30000.", "Enter frequency in Hz",
    "Create Square Wave", 256, 1, 30000, this);
  if (frequency != -1) {
    audiere::OutputStream* stream = m_device->openStream(audiere::CreateSquareWave(frequency));
    if (!stream) {
      wxMessageBox(
        "Could not open output stream",
        "Create Square Wave", wxOK | wxCENTRE, this);
    } else {
      wxString title;
      title.sprintf("Square Wave: %d Hz", frequency);
      new StreamFrame(this, title, stream);
    }
  }
}


void DeviceFrame::OnDeviceCreateWhiteNoise() {
  audiere::OutputStream* stream = m_device->openStream(audiere::CreateWhiteNoise());
  if (!stream) {
    wxMessageBox(
      "Could not open output stream",
      "Create White Noise", wxOK | wxCENTRE, this);
  } else {
    new StreamFrame(this, "White Noise", stream);
  }
}


void DeviceFrame::OnDeviceCreatePinkNoise() {
  audiere::OutputStream* stream = m_device->openStream(audiere::CreatePinkNoise());
  if (!stream) {
    wxMessageBox(
      "Could not open output stream",
      "Create Pink Noise", wxOK | wxCENTRE, this);
  } else {
    new StreamFrame(this, "Pink Noise", stream);
  }
}


void DeviceFrame::OnDeviceOpenSingleEffect() {
  DoOpenEffect(audiere::SINGLE, "Single");
}


void DeviceFrame::OnDeviceOpenMultipleEffect() {
  DoOpenEffect(audiere::MULTIPLE, "Multiple");
}


void DeviceFrame::DoOpenEffect(audiere::SoundEffectType type, wxString typestring) {
  wxString filename(GetSoundFile());
  if (filename.empty()) {
    return;
  }

  audiere::SoundEffect* effect = audiere::OpenSoundEffect(m_device, filename, type);
  if (effect) {
    wxString basename = wxFileNameFromPath(filename);
    wxString title;
    title.sprintf("Sound Effect (%s): %s",
                  typestring.c_str(), basename.c_str());
    new SoundEffectFrame(this, title, effect);
  } else {
    wxMessageBox(
      "Could not open sound effect: " + filename,
      "Open Sound Effect", wxOK | wxCENTRE, this);
  }
}


void DeviceFrame::OnDeviceCloseCurrentWindow() {
  wxMDIChildFrame* frame = GetActiveChild();
  if (frame) {
    frame->Close();
  }
}


void DeviceFrame::OnDeviceClose() {
  Close();
}


void DeviceFrame::OnHelpAbout() {
  wxString message =
    "wxPlayer\n"
    "Copyright (c) Chad Austin 2002-2003\n\n"
    "Audiere Version: ";
  message += audiere::GetVersion();
  wxMessageBox(message, "About wxPlayer", wxOK | wxCENTRE, this);
}
