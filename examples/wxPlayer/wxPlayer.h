#ifndef ADR_WX_PLAYER_H
#define ADR_WX_PLAYER_H


#include <string>
#include "wx.h"


class wxPlayer : public wxApp {
public:
  bool OnInit();
  bool TryDevice(const std::string& name, const std::string& parameters = "");
};


DECLARE_APP(wxPlayer)


#endif
