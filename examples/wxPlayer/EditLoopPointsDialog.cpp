#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include "Commands.h"
#include "EditLoopPointsDialog.h"


BEGIN_EVENT_TABLE(EditLoopPointsDialog, wxDialog)
  EVT_CLOSE(EditLoopPointsDialog::OnClose)
  EVT_BUTTON(-1, EditLoopPointsDialog::OnButton)
END_EVENT_TABLE()


EditLoopPointsDialog::EditLoopPointsDialog(
  wxWindow* parent,
  audiere::LoopPointSourcePtr loop_source)
: wxDialog(parent, -1, wxString("Edit Loop Points"), wxDefaultPosition,
           wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  m_loop_source = loop_source;

  m_points     = new wxListBox(this, -1);
  m_location   = new wxTextCtrl(this, -1);
  m_target     = new wxTextCtrl(this, -1);
  m_loop_count = new wxTextCtrl(this, -1);
  m_add        = new wxButton(this, -1, "Add");
  m_remove     = new wxButton(this, -1, "Remove");
  m_close      = new wxButton(this, -1, "Close");

  const int CA = wxALIGN_CENTER | wxALL;

  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(m_add,    0, CA, 5);
  buttonSizer->Add(m_remove, 0, CA, 5);

  const int LEFT_STYLE = CA | wxALIGN_CENTER_VERTICAL;

  wxFlexGridSizer* fieldSizer = new wxFlexGridSizer(2);
  fieldSizer->Add(new wxStaticText(this, -1, "Location"),   0, LEFT_STYLE, 0);
  fieldSizer->Add(m_location,   0, CA, 1);
  fieldSizer->Add(new wxStaticText(this, -1, "Target"),     0, LEFT_STYLE, 0);
  fieldSizer->Add(m_target,     0, CA, 1);
  fieldSizer->Add(new wxStaticText(this, -1, "Loop Count"), 0, LEFT_STYLE, 0);
  fieldSizer->Add(m_loop_count, 0, CA, 1);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(m_points,     1, CA | wxGROW | wxADJUST_MINSIZE, 5);
  sizer->Add(fieldSizer,   0, CA, 5);
  sizer->Add(buttonSizer,  0, CA, 2);
  sizer->Add(m_close,      0, CA, 1);

  m_location->SetFocus();
  m_add->SetDefault();

  SetAutoLayout(true);
  SetSizer(sizer);

  sizer->Fit(this);
  sizer->SetSizeHints(this);

  resetFields();
  refreshLoopPoints();
}


void EditLoopPointsDialog::OnButton(wxCommandEvent& event) {
  if (event.GetEventObject() == m_add) {

    int location   = atoi(m_location  ->GetValue());
    int target     = atoi(m_target    ->GetValue());
    int loop_count = atoi(m_loop_count->GetValue());
    m_loop_source->addLoopPoint(location, target, loop_count);

    resetFields();
    refreshLoopPoints();

  } else if (event.GetEventObject() == m_remove) {

    int sel = m_points->GetSelection();
    if (sel >= 0 && sel < m_loop_source->getLoopPointCount()) {
      m_loop_source->removeLoopPoint(sel);
      refreshLoopPoints();
    }
    
  } else if (event.GetEventObject() == m_close) {

    EndModal(0);

  }
}


void EditLoopPointsDialog::OnClose(wxCloseEvent& event) {
  EndModal(0);
}


void EditLoopPointsDialog::resetFields() {
  m_location  ->SetValue("0");
  m_target    ->SetValue("0");
  m_loop_count->SetValue("0");
}


void EditLoopPointsDialog::refreshLoopPoints() {
  m_points->Clear();
  int count = m_loop_source->getLoopPointCount();
  for (int i = 0; i < count; ++i) {
    int location, target, loopCount;
    if (m_loop_source->getLoopPoint(i, location, target, loopCount)) {
      wxString str;
      str.Printf("%d  %d  %d", location, target, loopCount);
      m_points->Append(str);
    }
  }
}
