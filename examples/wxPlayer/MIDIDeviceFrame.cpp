#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include "Commands.h"
#include "MIDIDeviceFrame.h"
#include "MIDIStreamFrame.h"
#include "wxPlayer.h"


BEGIN_EVENT_TABLE(MIDIDeviceFrame, wxMDIParentFrame)
  EVT_MENU(DEVICE_NEW_DEVICE,     MIDIDeviceFrame::OnDeviceNewDevice)
  EVT_MENU(DEVICE_NEW_CDDEVICE,   MIDIDeviceFrame::OnDeviceNewCDDevice)
  EVT_MENU(DEVICE_NEW_MIDIDEVICE, MIDIDeviceFrame::OnDeviceNewMIDIDevice)
  EVT_MENU(DEVICE_OPEN_SONG,      MIDIDeviceFrame::OnDeviceOpenSong)
  EVT_MENU(DEVICE_CLOSE_WINDOW,   MIDIDeviceFrame::OnDeviceCloseCurrentWindow)
  EVT_MENU(DEVICE_CLOSE,          MIDIDeviceFrame::OnDeviceClose)
  EVT_MENU(HELP_ABOUT,            MIDIDeviceFrame::OnHelpAbout)
END_EVENT_TABLE()


MIDIDeviceFrame::MIDIDeviceFrame(audiere::MIDIDevicePtr device)
: wxMDIParentFrame(0, -1, "MIDI Device - " + wxString(device->getName()))
{
  m_device = device;

  wxMenu* deviceMenu = new wxMenu;
  deviceMenu->Append(DEVICE_NEW_DEVICE,     "&New Device...");
  deviceMenu->Append(DEVICE_NEW_CDDEVICE,   "New C&D Device...");
  deviceMenu->Append(DEVICE_NEW_MIDIDEVICE, "New &MIDI Device...");
  deviceMenu->AppendSeparator();
  deviceMenu->Append(DEVICE_OPEN_SONG,      "Open &Song...");
  deviceMenu->AppendSeparator();
  deviceMenu->Append(DEVICE_CLOSE_WINDOW,   "Close C&urrent Window");
  deviceMenu->Append(DEVICE_CLOSE,          "&Close Device");
  
  wxMenu* helpMenu = new wxMenu;
  helpMenu->Append(HELP_ABOUT, "&About...");

  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(deviceMenu, "&Device");
  menuBar->Append(helpMenu,   "&Help");
  SetMenuBar(menuBar);

  SetFocus();
}


void MIDIDeviceFrame::OnDeviceNewDevice() {
  wxGetApp().OnNewDevice(this);
}


void MIDIDeviceFrame::OnDeviceNewCDDevice() {
  wxGetApp().OnNewCDDevice(this);
}


void MIDIDeviceFrame::OnDeviceNewMIDIDevice() {
  wxGetApp().OnNewMIDIDevice(this);
}


void MIDIDeviceFrame::OnDeviceOpenSong() {
  wxString file = wxFileSelector(
    "Select a MIDI file", "", "", "",
    "MIDI Files (*.midi,*.mid,*.rmi)|*.midi;*.mid;*.rmi|"
    "All Files (*.*)|*.*",
    wxOPEN, this);
  if (!file.empty()) {
    audiere::MIDIStreamPtr stream = m_device->openStream(file.c_str());
    if (!stream) {
      wxMessageBox(
        "Could not open MIDI file: " + file,
        "Open MIDI File", wxOK | wxCENTRE, this);
    }

    new MIDIStreamFrame(this, "MIDI File - " + file, stream);
  }
}


void MIDIDeviceFrame::OnDeviceCloseCurrentWindow() {
  wxMDIChildFrame* frame = GetActiveChild();
  if (frame) {
    frame->Close();
  }
}


void MIDIDeviceFrame::OnDeviceClose() {
  Close();
}


void MIDIDeviceFrame::OnHelpAbout() {
  wxGetApp().ShowAboutDialog(this);
}
