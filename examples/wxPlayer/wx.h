#ifndef ADR_WX_H
#define ADR_WX_H


#ifdef _MSC_VER
  #pragma warning(disable : 4786)
  #include <windows.h>
  #ifdef CreateDialog
    #undef CreateDialog
  #endif
#endif

#include <wx/wx.h>


#endif
