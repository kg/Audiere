#ifndef ADR_MIDI_STREAM_FRAME_H
#define ADR_MIDI_STREAM_FRAME_H


#include <audiere.h>
#include "wx.h"


class MIDIStreamFrame : public wxMDIChildFrame {
public:
  MIDIStreamFrame(
    wxMDIParentFrame* parent,
    const wxString& title,
    audiere::MIDIStreamPtr stream);
  ~MIDIStreamFrame();

  void OnPlay();
  void OnPause();
  void OnStop();

  void OnRepeat();
  void OnChangePos();
  void OnUpdateStatus();

private:
  audiere::MIDIStreamPtr m_stream;

  wxTimer* m_timer;

  wxCheckBox*   m_repeating;
  wxStaticText* m_is_playing_label;
  wxStaticText* m_length_pos_label;
  wxSlider*     m_pos;

  DECLARE_EVENT_TABLE()
};


#endif
