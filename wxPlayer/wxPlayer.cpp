#include <windows.h>

#include <vector>
#include <wx/wx.h>
#include <audiere.h>


static ADR_CONTEXT g_context;


class wxPlayerFrame : public wxFrame
{
public:
  wxPlayerFrame();

  void OnClose(wxCloseEvent& event);
  void OnSize(wxSizeEvent& event);
  
  void OnLoad(wxCommandEvent& event);
  void OnDump(wxCommandEvent& event);
  void OnPlay(wxCommandEvent& event);
  void OnPause(wxCommandEvent& event);
  void OnReset(wxCommandEvent& event);
  void OnRepeat(wxCommandEvent& event);

  void OnChangeVolume(wxScrollEvent& event);
  void OnChangePan(wxScrollEvent& event);

private:
  wxListBox* m_songs;
  wxButton* m_load;
  wxButton* m_dump;
  wxButton* m_play;
  wxButton* m_pause;
  wxButton* m_reset;
  wxButton* m_repeat;
  wxSlider* m_volume;
  wxSlider* m_pan;

  std::vector<ADR_STREAM> m_streams;

  DECLARE_EVENT_TABLE()
};


enum {
  event_Begin = 100,

  event_Load,
  event_Dump,
  event_Play,
  event_Pause,
  event_Reset,
  event_Repeat,

  event_Volume,
  event_Pan,
};


BEGIN_EVENT_TABLE(wxPlayerFrame, wxFrame)

  EVT_CLOSE(wxPlayerFrame::OnClose)
  EVT_SIZE(wxPlayerFrame::OnSize)

  EVT_BUTTON(event_Load,   wxPlayerFrame::OnLoad)
  EVT_BUTTON(event_Dump,   wxPlayerFrame::OnDump)
  EVT_BUTTON(event_Play,   wxPlayerFrame::OnPlay)
  EVT_BUTTON(event_Pause,  wxPlayerFrame::OnPause)
  EVT_BUTTON(event_Reset,  wxPlayerFrame::OnReset)
  EVT_BUTTON(event_Repeat, wxPlayerFrame::OnRepeat)

  EVT_COMMAND_SCROLL(event_Volume, wxPlayerFrame::OnChangeVolume)
  EVT_COMMAND_SCROLL(event_Pan,    wxPlayerFrame::OnChangePan)
  
END_EVENT_TABLE()


wxPlayerFrame::wxPlayerFrame()
: wxFrame(NULL, -1, "wxPlayer", wxDefaultPosition, wxSize(400, 300))
{
  m_songs  = new wxListBox(this, -1, wxDefaultPosition, wxSize(300, 200));
  m_load   = new wxButton(this, event_Load,   "load",   wxDefaultPosition);
  m_dump   = new wxButton(this, event_Dump,   "dump",   wxDefaultPosition);
  m_play   = new wxButton(this, event_Play,   "play",   wxDefaultPosition);
  m_pause  = new wxButton(this, event_Pause,  "pause",  wxDefaultPosition);
  m_reset  = new wxButton(this, event_Reset,  "reset",  wxDefaultPosition);
  m_repeat = new wxButton(this, event_Repeat, "repeat", wxDefaultPosition);
  m_volume = new wxSlider(this, event_Volume, ADR_VOLUME_MAX, ADR_VOLUME_MIN, ADR_VOLUME_MAX);
  m_pan    = new wxSlider(this, event_Pan,    ADR_PAN_CENTER, ADR_PAN_LEFT, ADR_PAN_RIGHT);
}


void
wxPlayerFrame::OnClose(wxCloseEvent& event)
{
  // destroy all open streams
  for (int i = 0; i < m_streams.size(); i++) {
    AdrCloseStream(m_streams[i]);
  }

  Destroy();
}


void
wxPlayerFrame::OnSize(wxSizeEvent& event)
{
  static const int button_width = 80;
  static const int button_height = 30;

  wxSize size = event.GetSize();
  int button_x = size.GetWidth() - button_width;

  m_songs->SetSize(0, 0, button_x, size.GetHeight());

  // move the controls
  wxControl* controls[] = {
    m_load, m_dump, m_play, m_pause, m_reset, m_repeat, m_volume, m_pan
  };
  for (int i = 0; i < sizeof(controls) / sizeof(controls[0]); i++) {
    controls[i]->SetSize(button_x, button_height * i, button_width, button_height);
  }
}


void
wxPlayerFrame::OnLoad(wxCommandEvent& event)
{
  const wxString result = wxFileSelector(
    "Select a sound file",
    "",
    "",
    "",
    "Sound Files (*.wav,*.mp3,*.ogg,*.mod,*.it,*.xm,*.s3m)|*.wav;*.mp3;*.ogg;*.mod;*.it;*.xm;*.s3m",
    wxOPEN,
    this
  );
  if (!result.Length()) {
    return;
  }

  ADR_STREAM stream = AdrOpenStream(g_context, result.c_str());
  if (!stream) {
    wxMessageBox("Could not open stream");
  } else {
    m_songs->Append(result);
    m_streams.push_back(stream);
  }
}


void
wxPlayerFrame::OnDump(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    AdrCloseStream(m_streams[sel]);
    m_streams.erase(m_streams.begin() + sel);
    m_songs->Delete(sel);

  }
}


void
wxPlayerFrame::OnPlay(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    AdrPlayStream(m_streams[sel]);

  }
}


void
wxPlayerFrame::OnPause(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    AdrPauseStream(m_streams[sel]);

  }
}


void
wxPlayerFrame::OnReset(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    AdrResetStream(m_streams[sel]);

  }
}


void
wxPlayerFrame::OnRepeat(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    AdrSetStreamRepeat(m_streams[sel], !AdrGetStreamRepeat(m_streams[sel]));

  }
}


void
wxPlayerFrame::OnChangeVolume(wxScrollEvent& event)
{
  int volume = event.GetPosition();
  for (int i = 0; i < m_songs->Number(); i++) {
    AdrSetStreamVolume(m_streams[i], volume);
  }
}


void
wxPlayerFrame::OnChangePan(wxScrollEvent& event)
{
  int pan = event.GetPosition();
  for (int i = 0; i < m_songs->Number(); i++) {
    AdrSetStreamPan(m_streams[i], pan);
  }
}


////////////////////////////////////////////////////////////////////////////////


class wxPlayer : public wxApp
{
public:
  bool OnInit() {
    g_context = AdrCreateContext(
      "openal",
      "",
      NULL, NULL, NULL, NULL, NULL, NULL
    );
    if (!g_context) {
      wxMessageBox("Could not create Audiere context");
      return false;
    }

    wxPlayerFrame* frame = new wxPlayerFrame;
    frame->Show(true);
    SetTopWindow(frame);
    return true;
  }

  int OnExit() {
    if (g_context) {
      AdrDestroyContext(g_context);
    }
    return 0;
  }
};


IMPLEMENT_APP(wxPlayer)
