// We need to include windows.h to use Windows common controls without
// crashing?  What the heck...
//
// part #2: something to do with wxSlider's vtable generation.  VS.NET crashes
// as well.  Stupid wxWindows...  :P
#ifdef WIN32
#include <windows.h>
#endif

#include <vector>
#include <wx/wx.h>
#include <audiere.h>
using namespace audiere;
using namespace std;


static AudioDevice* g_device;


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

private:
  wxListBox* m_songs;
  wxButton* m_load;
  wxButton* m_dump;
  wxButton* m_play;
  wxButton* m_pause;
  wxButton* m_reset;
  wxButton* m_repeat;
  wxSlider* m_volume;

  vector<Sound*> m_streams;

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
  m_volume = new wxSlider(this, event_Volume, 1000, 0, 1000);
}


void
wxPlayerFrame::OnClose(wxCloseEvent& event)
{
  // destroy all open streams
  for (int i = 0; i < m_streams.size(); i++) {
    delete m_streams[i];
  }
  m_streams.clear();

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
    m_load, m_dump, m_play, m_pause, m_reset, m_repeat, m_volume
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
    "Sound Files (*.wav,*.ogg,*.mod,*.it,*.xm,*.s3m)|*.wav;*.ogg;*.mod;*.it;*.xm;*.s3m",
    wxOPEN,
    this
  );
  if (!result.Length()) {
    return;
  }

  SampleSource* source = OpenSampleSource(result.c_str());
  Sound* sound = OpenSound(g_device, source, STREAM);
  if (!sound) {
    wxMessageBox("Could not open stream");
  } else {
    m_songs->Append(result);
    m_streams.push_back(sound);
  }
}


void
wxPlayerFrame::OnDump(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    delete m_streams[sel];
    m_streams.erase(m_streams.begin() + sel);
    m_songs->Delete(sel);

  }
}


void
wxPlayerFrame::OnPlay(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    m_streams[sel]->play();

  }
}


void
wxPlayerFrame::OnPause(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    m_streams[sel]->stop();

  }
}


void
wxPlayerFrame::OnReset(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    m_streams[sel]->reset();

  }
}


void
wxPlayerFrame::OnRepeat(wxCommandEvent& event)
{
  int sel = m_songs->GetSelection();
  if (sel >= 0 && sel < m_songs->Number()) {

    m_streams[sel]->setRepeat(!m_streams[sel]->getRepeat());

  }
}


void
wxPlayerFrame::OnChangeVolume(wxScrollEvent& event)
{
  int volume = event.GetPosition();
  for (int i = 0; i < m_songs->Number(); i++) {
    m_streams[i]->setVolume(volume / 1000.0f);
  }
}


////////////////////////////////////////////////////////////////////////////////


class wxPlayer : public wxApp
{
public:
  bool OnInit() {
    g_device = audiere::OpenDevice();
    if (!g_device) {
      wxMessageBox("Could not open output device");
      return false;
    }

    wxPlayerFrame* frame = new wxPlayerFrame;
    frame->Show(true);
    SetTopWindow(frame);
    return true;
  }

  int OnExit() {
    delete g_device;
    return 0;
  }
};


IMPLEMENT_APP(wxPlayer)
