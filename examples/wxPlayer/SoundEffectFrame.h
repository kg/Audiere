#ifndef ADR_SOUND_EFFECT_FRAME_H
#define ADR_SOUND_EFFECT_FRAME_H


#include <audiere.h>
#include "wx.h"


class SoundEffectFrame : public wxMDIChildFrame {
public:
  SoundEffectFrame(wxMDIParentFrame* parent, const wxString& title, audiere::SoundEffect* effect);

  void OnPlay();
  void OnStop();
  void OnChangeVolume();
  void OnChangePan();
  void OnChangePitch();
  void UpdateVPPLabel();

private:
  audiere::SoundEffectPtr m_effect;

  wxStaticText* m_vpp_label;
  wxSlider*     m_volume;
  wxSlider*     m_pan;
  wxSlider*     m_pitch;

  DECLARE_EVENT_TABLE()
};


#endif
