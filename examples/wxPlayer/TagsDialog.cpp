#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <algorithm>
#include "TagsDialog.h"


BEGIN_EVENT_TABLE(TagsDialog, wxDialog)
  EVT_CLOSE(TagsDialog::OnClose)
  EVT_BUTTON(-1, TagsDialog::OnButton)
END_EVENT_TABLE()


TagsDialog::TagsDialog(
  wxWindow* parent,
  audiere::SampleSource* source)
: wxDialog(parent, -1, wxString("View Tags"), wxDefaultPosition,
           wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  m_tags  = new wxListBox(this, -1);
  m_tags->SetFocus();

  std::vector<std::string> tags;
  for (int i = 0; i < source->getTagCount(); ++i) {
    std::string tag = source->getTagType(i);
    tag += ": ";
    tag += source->getTagKey(i);
    tag += "=";
    tag += source->getTagValue(i);
    tags.push_back(tag.c_str());
  }
  std::sort(tags.begin(), tags.end());
  for (size_t i = 0; i < tags.size(); ++i) {
    m_tags->Append(tags[i].c_str());
  }

  m_close = new wxButton(this, -1, "Close");
  m_close->SetDefault();

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(m_tags,  1, wxADJUST_MINSIZE | wxGROW | wxALIGN_CENTER | wxALL, 1);
  sizer->Add(m_close, 0, wxALIGN_CENTER | wxALL, 1);

  SetAutoLayout(true);
  SetSizer(sizer);

  sizer->Fit(this);
  sizer->SetSizeHints(this);
}


void TagsDialog::OnButton(wxCommandEvent& event) {
  EndModal(0);
}


void TagsDialog::OnClose(wxCloseEvent& event) {
  EndModal(0);
}
