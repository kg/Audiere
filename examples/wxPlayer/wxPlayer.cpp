#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <set>
#include <wx/wx.h>
#include <audiere.h>
using namespace audiere;


bool TryDevice(const wxString& name, const wxString& parameters = "");


enum {
  DEVICE_NEW_DEVICE,
  DEVICE_OPEN_STREAM,
  DEVICE_OPEN_SOUND,
  DEVICE_CREATE_TONE,
  DEVICE_CREATE_SQUARE_WAVE,
  DEVICE_CREATE_WHITE_NOISE,
  DEVICE_CREATE_PINK_NOISE,
  DEVICE_OPEN_SINGLE_EFFECT,
  DEVICE_OPEN_MULTIPLE_EFFECT,
  DEVICE_CLOSE_WINDOW,
  DEVICE_CLOSE,

  HELP_ABOUT,

  STREAM_PLAY,
  STREAM_STOP,
  STREAM_RESET,
  STREAM_REPEAT,
  STREAM_VOLUME,
  STREAM_PAN,
  STREAM_PITCH,
  STREAM_POS,
  STREAM_UPDATE,

  EFFECT_PLAY,
  EFFECT_STOP,
  EFFECT_VOLUME,
  EFFECT_PAN,
  EFFECT_PITCH,
};


class StreamFrame : public wxMDIChildFrame {
public:
  StreamFrame(wxMDIParentFrame* parent, const wxString& title, OutputStream* stream)
  : wxMDIChildFrame(parent, -1, title)
  {
    m_stream = stream;
    m_stream_is_seekable = stream->isSeekable();
    m_stream_length = stream->getLength();

    m_playing_label = new wxStaticText(this, -1, "Stopped");
    m_repeating = new wxCheckBox(this, STREAM_REPEAT, "Repeating");
    m_vpp_label = new wxStaticText(this, -1, "Volume - Pan - Pitch");
    m_volume = new wxSlider(this, STREAM_VOLUME, 100, 0,  100);
    m_pan    = new wxSlider(this, STREAM_PAN,    0, -100, 100);
    m_pitch  = new wxSlider(this, STREAM_PITCH,  100, 50, 200);
    m_length_pos_label = new wxStaticText(this, -1, "Length - Pos");
    m_pos    = new wxSlider(this, STREAM_POS,    0, 0, 1000);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(
      new wxButton(this, STREAM_PLAY, "Play"),
      1, wxEXPAND | wxALL, 4);
    sizer->Add(
      new wxButton(this, STREAM_STOP, "Stop"),
      1, wxEXPAND | wxALL, 4);
    sizer->Add(
      new wxButton(this, STREAM_RESET, "Reset"),
      1, wxEXPAND | wxALL, 4);
    sizer->Add(m_playing_label,    1, wxEXPAND | wxALL, 4);
    sizer->Add(m_repeating,        1, wxEXPAND | wxALL, 4);
    sizer->Add(m_vpp_label,        1, wxEXPAND | wxALL, 4);
    sizer->Add(m_volume,           1, wxEXPAND | wxALL, 4);
    sizer->Add(m_pan,              1, wxEXPAND | wxALL, 4);
    sizer->Add(m_pitch,            1, wxEXPAND | wxALL, 4);
    sizer->Add(m_length_pos_label, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_pos,              1, wxEXPAND | wxALL, 4);

    if (!m_stream_is_seekable) {
      m_pos->Enable(false);
    }

    UpdateVPPLabel();
    UpdateLengthPosLabel();

    SetAutoLayout(true);
    SetSizer(sizer);

    sizer->Fit(this);
    sizer->SetSizeHints(this);

    // create a timer to update the current position
    m_timer = new wxTimer(this, STREAM_UPDATE);
    m_timer->Start(500);
  }

  ~StreamFrame() {
    m_timer->Stop();
    delete m_timer;
  }

  void OnPlay() {
    m_stream->play();
  }

  void OnStop() {
    m_stream->stop();
  }

  void OnReset() {
    m_stream->reset();
  }

  void OnRepeat() {
    m_stream->setRepeat(m_repeating->GetValue());
  }

  void OnChangeVolume() {
    m_stream->setVolume(m_volume->GetValue() / 100.0f);
    UpdateVPPLabel();
  }

  void OnChangePan() {
    m_stream->setPan(m_pan->GetValue() / 100.0f);
    UpdateVPPLabel();
  }

  void OnChangePitch() {
    m_stream->setPitchShift(m_pitch->GetValue() / 100.0f);
    UpdateVPPLabel();
  }

  void OnChangePos() {
    float pos = float(m_pos->GetValue()) / 1000;
    m_stream->setPosition(int(pos * m_stream_length));
  }

  void OnUpdateStatus() {
    if (m_stream_is_seekable) {
      float pos = float(m_stream->getPosition()) / m_stream_length;
      m_pos->SetValue(int(1000 * pos));
    }
    UpdateLengthPosLabel();

    // update the playing label
    m_playing_label->SetLabel(m_stream->isPlaying() ? "Playing" : "Stopped");
  }

  void UpdateVPPLabel() {
    wxString label;
    label.Printf(
      "Vol: %1.2f  Pan: %1.2f  Pitch: %1.2f",
      m_volume->GetValue() / 100.0f,
      m_pan->GetValue() / 100.0f,
      m_pitch->GetValue() / 100.0f);
    m_vpp_label->SetLabel(label);
  }

  void UpdateLengthPosLabel() {
    if (m_stream_is_seekable) {
      wxString label;
      label.Printf(
        "Length: %d   Pos: %d",
        m_stream_length,
        m_stream->getPosition());
      m_length_pos_label->SetLabel(label);
    } else {
      m_length_pos_label->SetLabel("Unseekable Stream");
    }
  }

private:
  OutputStreamPtr m_stream;
  bool m_stream_is_seekable;
  int m_stream_length;

  wxTimer* m_timer;

  wxStaticText* m_playing_label;
  wxCheckBox*   m_repeating;
  wxStaticText* m_vpp_label;
  wxSlider*     m_volume;
  wxSlider*     m_pan;
  wxSlider*     m_pitch;
  wxStaticText* m_length_pos_label;
  wxSlider*     m_pos;

  DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(StreamFrame, wxMDIChildFrame)
  EVT_BUTTON(STREAM_PLAY,  StreamFrame::OnPlay)
  EVT_BUTTON(STREAM_STOP,  StreamFrame::OnStop)
  EVT_BUTTON(STREAM_RESET, StreamFrame::OnReset)

  EVT_CHECKBOX(STREAM_REPEAT, StreamFrame::OnRepeat)

  EVT_COMMAND_SCROLL(STREAM_VOLUME, StreamFrame::OnChangeVolume)
  EVT_COMMAND_SCROLL(STREAM_PAN,    StreamFrame::OnChangePan)
  EVT_COMMAND_SCROLL(STREAM_PITCH,  StreamFrame::OnChangePitch)
  EVT_COMMAND_SCROLL(STREAM_POS,    StreamFrame::OnChangePos)

  EVT_TIMER(STREAM_UPDATE, StreamFrame::OnUpdateStatus)
END_EVENT_TABLE()


class SoundEffectFrame : public wxMDIChildFrame {
public:
  SoundEffectFrame(wxMDIParentFrame* parent, const wxString& title, SoundEffect* effect)
  : wxMDIChildFrame(parent, -1, title)
  {
    m_effect = effect;

    m_vpp_label = new wxStaticText(this, -1, "Volume - Pan - Pitch");
    m_volume = new wxSlider(this, STREAM_VOLUME, 100, 0,  100);
    m_pan    = new wxSlider(this, STREAM_PAN,    0, -100, 100);
    m_pitch  = new wxSlider(this, STREAM_PITCH,  100, 50, 200);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(
      new wxButton(this, EFFECT_PLAY, "Play"),
      1, wxEXPAND | wxALL, 4);
    sizer->Add(
      new wxButton(this, EFFECT_STOP, "Stop"),
      1, wxEXPAND | wxALL, 4);
    sizer->Add(m_vpp_label, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_volume,    1, wxEXPAND | wxALL, 4);
    sizer->Add(m_pan,       1, wxEXPAND | wxALL, 4);
    sizer->Add(m_pitch,     1, wxEXPAND | wxALL, 4);

    UpdateVPPLabel();

    SetAutoLayout(true);
    SetSizer(sizer);

    sizer->Fit(this);
    sizer->SetSizeHints(this);
  }

  void OnPlay() {
    m_effect->play();
  }

  void OnStop() {
    m_effect->stop();
  }

  void OnChangeVolume() {
    m_effect->setVolume(m_volume->GetValue() / 100.0f);
    UpdateVPPLabel();
  }

  void OnChangePan() {
    m_effect->setPan(m_pan->GetValue() / 100.0f);
    UpdateVPPLabel();
  }

  void OnChangePitch() {
    m_effect->setPitchShift(m_pitch->GetValue() / 100.0f);
    UpdateVPPLabel();
  }

  void UpdateVPPLabel() {
    wxString label;
    label.Printf(
      "Vol: %1.2f  Pan: %1.2f  Pitch: %1.2f",
      m_volume->GetValue() / 100.0f,
      m_pan->GetValue() / 100.0f,
      m_pitch->GetValue() / 100.0f);
    m_vpp_label->SetLabel(label);
  }

private:
  SoundEffectPtr m_effect;

  wxStaticText* m_vpp_label;
  wxSlider*     m_volume;
  wxSlider*     m_pan;
  wxSlider*     m_pitch;

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(SoundEffectFrame, wxMDIChildFrame)
  EVT_BUTTON(EFFECT_PLAY,  SoundEffectFrame::OnPlay)
  EVT_BUTTON(EFFECT_STOP,  SoundEffectFrame::OnStop)

  EVT_COMMAND_SCROLL(STREAM_VOLUME, SoundEffectFrame::OnChangeVolume)
  EVT_COMMAND_SCROLL(STREAM_PAN,    SoundEffectFrame::OnChangePan)
  EVT_COMMAND_SCROLL(STREAM_PITCH,  SoundEffectFrame::OnChangePitch)
END_EVENT_TABLE()


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
      return result;
    } else {
      result += joiner;
    }
  }
}


class NewDeviceDialog : public wxDialog {
public:
  NewDeviceDialog(wxWindow* parent)
    : wxDialog(parent, -1, wxString("New Device"))
  {
    // size of text controls
    static const wxSize size(300, 22);

    GetSupportedAudioDevices(m_devices);

    m_device = new wxChoice(this, -1, wxDefaultPosition, size);
    m_device->Append("autodetect: Choose default device");
    for (unsigned i = 0; i < m_devices.size(); ++i) {
      m_device->Append((m_devices[i].name + ": " + m_devices[i].description).c_str());
    }
    m_device->SetSelection(0);

    // create parameters box
    m_parameters = new wxTextCtrl(this, -1, "", wxDefaultPosition, size);

    // create vertical sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_device,     0, wxALIGN_CENTER | wxALL, 5);
    sizer->Add(m_parameters, 0, wxALIGN_CENTER | wxALL, 5);

    // button bar
    m_ok     = new wxButton(this, -1, "OK");
    m_cancel = new wxButton(this, -1, "Cancel");
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(m_ok,     0, wxALIGN_CENTER | wxALL, 5);
    buttonSizer->Add(m_cancel, 0, wxALIGN_CENTER | wxALL, 5);

    sizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 4);

    SetAutoLayout(TRUE);
    SetSizer(sizer);

    m_device->SetFocus();
    m_ok->SetDefault();

    sizer->Fit(this);
    sizer->SetSizeHints(this);
  }

  const std::string& getDevice() const {
    return m_device_str;
  }

  const std::string& getParameters() const {
    return m_parameters_str;
  }

private:
  void OnButton(wxCommandEvent& event) {
    if (event.GetEventObject() == m_ok) {
      int value = m_device->GetSelection();
      if (value == 0) {
        m_device_str = "autodetect";
      } else {
        m_device_str = m_devices[value - 1].name;
      }

      m_parameters_str = m_parameters->GetValue();
      EndModal(wxID_OK);
    } else if (event.GetEventObject() == m_cancel) {
      EndModal(wxID_CANCEL);
    }
  }

  std::vector<AudioDeviceDesc> m_devices;

  wxChoice*   m_device;
  wxTextCtrl* m_parameters;

  wxButton* m_ok;
  wxButton* m_cancel;

  std::string m_device_str;
  std::string m_parameters_str;

  DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(NewDeviceDialog, wxDialog)
  EVT_BUTTON(-1, NewDeviceDialog::OnButton)
END_EVENT_TABLE()


class DeviceFrame : public wxMDIParentFrame {
public:
  DeviceFrame(AudioDevice* device)
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

  void OnDeviceNewDevice() {
    NewDeviceDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK) {
      bool result = TryDevice(dialog.getDevice().c_str(),
                              dialog.getParameters().c_str());
      if (!result) {
        wxMessageBox("Error opening new device", "New Device",
                     wxOK | wxICON_ERROR, this);
      }
    }
  }

  wxString GetSoundFile() {
    std::vector<FileFormatDesc> formats;
    GetSupportedFileFormats(formats);

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
      FileFormatDesc& ffd = formats[i];
      wildcards += ffd.description + " ";
      wildcards += "(" + Join(ffd.extensions, ",", "*.") + ")|";
      wildcards += Join(ffd.extensions, ";", "*.") + "|";
    }

    wildcards += "All Files (*.*)|*.*";

    return wxFileSelector(
      "Select a sound file", "", "", "",
      wildcards.c_str(), wxOPEN, this);
  }

  void OnDeviceOpenStream() {
    wxString filename(GetSoundFile());
    if (filename.empty()) {
      return;
    }

    OutputStream* stream = OpenSound(m_device, filename, true);
    if (!stream) {
      wxMessageBox(
        "Could not open stream: " + filename,
        "Open Stream", wxOK | wxCENTRE, this);
      return;
    }

    // get the basename of the path for the window title
    wxString basename = wxFileNameFromPath(filename);

    wxString title;
    title.sprintf("Stream: %s", basename.c_str());
    new StreamFrame(this, title, stream);
  }

  void OnDeviceOpenSound() {
    wxString filename(GetSoundFile());
    if (filename.empty()) {
      return;
    }

    OutputStream* stream = OpenSound(m_device, filename);
    if (!stream) {
      wxMessageBox(
        "Could not open sound: " + filename,
        "Open Stream", wxOK | wxCENTRE, this);
      return;
    }

    // get the basename of the path for the window title
    wxString basename = wxFileNameFromPath(filename);;

    wxString title;
    title.sprintf("Sound: %s", basename.c_str());
    new StreamFrame(this, title, stream);
  }

  void OnDeviceCreateTone() {
    int frequency = wxGetNumberFromUser(
      "Value must be between 1 and 30000.", "Enter frequency in Hz",
      "Create Tone", 256, 1, 30000, this);
    if (frequency != -1) {
      OutputStream* stream = m_device->openStream(CreateTone(frequency));
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

  void OnDeviceCreateSquareWave() {
    int frequency = wxGetNumberFromUser(
      "Value must be between 1 and 30000.", "Enter frequency in Hz",
      "Create Square Wave", 256, 1, 30000, this);
    if (frequency != -1) {
      OutputStream* stream = m_device->openStream(CreateSquareWave(frequency));
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

  void OnDeviceCreateWhiteNoise() {
    OutputStream* stream = m_device->openStream(CreateWhiteNoise());
    if (!stream) {
      wxMessageBox(
        "Could not open output stream",
        "Create White Noise", wxOK | wxCENTRE, this);
    } else {
      new StreamFrame(this, "White Noise", stream);
    }
  }

  void OnDeviceCreatePinkNoise() {
    OutputStream* stream = m_device->openStream(CreatePinkNoise());
    if (!stream) {
      wxMessageBox(
        "Could not open output stream",
        "Create Pink Noise", wxOK | wxCENTRE, this);
    } else {
      new StreamFrame(this, "Pink Noise", stream);
    }
  }

  void OnDeviceOpenSingleEffect() {
    DoOpenEffect(SINGLE, "Single");
  }

  void OnDeviceOpenMultipleEffect() {
    DoOpenEffect(MULTIPLE, "Multiple");
  }

  void DoOpenEffect(SoundEffectType type, wxString typestring) {
    wxString filename(GetSoundFile());
    if (filename.empty()) {
      return;
    }

    SoundEffect* effect = OpenSoundEffect(m_device, filename, type);
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

  void OnDeviceCloseCurrentWindow() {
    wxMDIChildFrame* frame = GetActiveChild();
    if (frame) {
      frame->Close();
    }
  }

  void OnDeviceClose() {
    Close();
  }

  void OnHelpAbout() {
    wxString message =
      "wxPlayer\n"
      "Copyright (c) Chad Austin 2002-2003\n\n"
      "Audiere Version: ";
    message += GetVersion();
    wxMessageBox(message, "About wxPlayer", wxOK | wxCENTRE, this);
  }

private:
  AudioDevicePtr m_device;

  DECLARE_EVENT_TABLE()
};


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


bool TryDevice(const wxString& name, const wxString& parameters) {
  if (AudioDevicePtr device = audiere::OpenDevice(name, parameters)) {
    DeviceFrame* frame = new DeviceFrame(device.get());
    frame->Show(true);
    return true;
  } else {
    return false;
  }
}


class wxPlayer : public wxApp {
public:
  bool OnInit() {
    bool result = (TryDevice("autodetect") || TryDevice("null"));
    if (!result) {
      wxMessageBox("Could not open output device", "wxPlayer");
    }
    return result;
  }
};


IMPLEMENT_APP(wxPlayer)
