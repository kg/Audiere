// for some reason, we have to include windows.h for this to run
#ifdef _MSC_VER
#include <windows.h>
#endif


#include <wx/wx.h>
#include <audiere.h>
using namespace audiere;


bool TryDevice(const char* name);


enum {
  DEVICE_NEW_DEVICE,
  DEVICE_OPEN_STREAM,
  DEVICE_OPEN_SOUND,
  DEVICE_CREATE_TONE,
  DEVICE_CLOSE,

  STREAM_PLAY,
  STREAM_STOP,
  STREAM_RESET,
  STREAM_REPEAT,
  STREAM_VOLUME,
  STREAM_PAN,
  STREAM_POS,
  STREAM_UPDATE,
};


class StreamFrame : public wxMDIChildFrame {
public:
  StreamFrame(wxMDIParentFrame* parent, wxString& title, OutputStream* stream)
  : wxMDIChildFrame(parent, -1, title)
  {
    m_stream = stream;

    int slider_max = (m_stream->isSeekable() ? m_stream->getLength() : 1);
    
    m_playing_label = new wxStaticText(this, -1, "Stopped");
    m_repeating = new wxCheckBox(this, STREAM_REPEAT, "Repeating");
    m_volume_pan_label = new wxStaticText(this, -1, "Volume - Pan");
    m_volume = new wxSlider(this, STREAM_VOLUME, 100, 0,  100);
    m_pan    = new wxSlider(this, STREAM_PAN,    0, -100, 100);
    m_length_pos_label = new wxStaticText(this, -1, "Length - Pos");
    m_pos    = new wxSlider(this, STREAM_POS,    0, 0, slider_max);

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
    sizer->Add(m_playing_label, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_repeating, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_volume_pan_label, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_volume, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_pan, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_length_pos_label, 1, wxEXPAND | wxALL, 4);
    sizer->Add(m_pos, 1, wxEXPAND | wxALL, 4);

    if (!m_stream->isSeekable()) {
      m_pos->Enable(false);
    }

    UpdateVolumePanLabel();
    UpdateLengthPosLabel();

    SetAutoLayout(true);
    SetSizer(sizer);

    sizer->Fit(this);
    sizer->SetSizeHints(this);

    // create a timer to update the current position
    m_timer = new wxTimer(this, STREAM_UPDATE);
    m_timer->Start(200);
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
    UpdateVolumePanLabel();
  }

  void OnChangePan() {
    m_stream->setPan(m_pan->GetValue() / 100.0f);
    UpdateVolumePanLabel();
  }

  void OnChangePos() {
    m_stream->setPosition(m_pos->GetValue());
  }

  void OnUpdateStatus() {
    if (m_stream->isSeekable()) {
      m_pos->SetValue(m_stream->getPosition());
    }
    UpdateLengthPosLabel();

    // update the playing label
    m_playing_label->SetLabel(m_stream->isPlaying() ? "Playing" : "Stopped");
  }

  void UpdateVolumePanLabel() {
    wxString label;
    label.Printf(
      "Volume: %1.2f   Pan: %1.2f",
      m_volume->GetValue() / 100.0f,
      m_pan->GetValue() / 100.0f);
    m_volume_pan_label->SetLabel(label);
  }

  void UpdateLengthPosLabel() {
    if (m_stream->isSeekable()) {
      wxString label;
      label.Printf(
        "Length: %d   Pos: %d",
        m_stream->getLength(),
        m_stream->getPosition());
      m_length_pos_label->SetLabel(label);
    } else {
      m_length_pos_label->SetLabel("Unseekable Stream");
    }
  }

private:
  RefPtr<OutputStream> m_stream;

  wxTimer* m_timer;

  wxStaticText* m_playing_label;
  wxCheckBox*   m_repeating;
  wxStaticText* m_volume_pan_label;
  wxSlider*     m_volume;
  wxSlider*     m_pan;
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
  EVT_COMMAND_SCROLL(STREAM_POS,    StreamFrame::OnChangePos)

  EVT_TIMER(STREAM_UPDATE, StreamFrame::OnUpdateStatus)
END_EVENT_TABLE()


class DeviceFrame : public wxMDIParentFrame {
public:
  DeviceFrame(AudioDevice* device, const wxString& device_name)
  : wxMDIParentFrame(NULL, -1, "Device Window - " + device_name)
  {
    m_device = device;

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(DEVICE_NEW_DEVICE,  "&New Device...");
    fileMenu->AppendSeparator();
    fileMenu->Append(DEVICE_OPEN_STREAM, "&Open Stream...");
    fileMenu->Append(DEVICE_OPEN_SOUND,  "Open &Sound...");
    fileMenu->Append(DEVICE_CREATE_TONE, "Create &Tone...");
    fileMenu->AppendSeparator();
    fileMenu->Append(DEVICE_CLOSE,       "&Close Device");

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&Device");
    SetMenuBar(menuBar);
  }

  void OnDeviceNewDevice() {
    wxString name = wxGetTextFromUser(
      "New Device",
      "Enter Device Name",
      "autodetect",
      this);
    if (!name.empty()) {
      if (!TryDevice(name)) {
        wxMessageBox(
          "Could not open audio device: " + name,
          "New Device", wxOK | wxCENTRE, this);
      }
    }
  }

  wxString GetSoundFile() {
    return wxFileSelector(
      "Select a sound file", "", "", "",
      "Sound Files (*.wav,*.ogg,*.mod,*.it,*.xm,*.s3m)|"  \
      "*.wav;*.ogg;*.mod;*.it;*.xm;*.s3m",
      wxOPEN, this);
  }

  void OnDeviceOpenStream() {
    wxString filename(GetSoundFile());
    if (filename.empty()) {
      return;
    }

    OutputStream* stream = OpenSound(m_device.get(), filename, true);
    if (!stream) {
      wxMessageBox(
        "Could not open stream: " + filename,
        "Open Stream", wxOK | wxCENTRE, this);
      return;
    }

    // get the basename of the path for the window title
    wxString basename = wxFileNameFromPath(filename);;

    wxString title;
    title.sprintf("Stream: %s", basename.c_str());
    new StreamFrame(this, title, stream);
  }

  void OnDeviceOpenSound() {
    wxString filename(GetSoundFile());
    if (filename.empty()) {
      return;
    }

    OutputStream* stream = OpenSound(m_device.get(), filename);
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

  void OnDeviceClose() {
    Close();
  }

private:
  RefPtr<AudioDevice> m_device;

  DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(DeviceFrame, wxMDIParentFrame)
  EVT_MENU(DEVICE_NEW_DEVICE,  DeviceFrame::OnDeviceNewDevice)
  EVT_MENU(DEVICE_OPEN_STREAM, DeviceFrame::OnDeviceOpenStream)
  EVT_MENU(DEVICE_OPEN_SOUND,  DeviceFrame::OnDeviceOpenSound)
  EVT_MENU(DEVICE_CREATE_TONE, DeviceFrame::OnDeviceCreateTone)
  EVT_MENU(DEVICE_CLOSE,       DeviceFrame::OnDeviceClose)
END_EVENT_TABLE()


bool TryDevice(const char* name) {
  if (RefPtr<AudioDevice> device = audiere::OpenDevice(name)) {
    DeviceFrame* frame = new DeviceFrame(device.get(), name);
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
