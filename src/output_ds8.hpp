#ifndef OUTPUT_DS8_HPP
#define OUTPUT_DS8_HPP


#include "output_ds.hpp"


// context
class DS8OutputContext : public DSOutputContext
{
public:
  DS8OutputContext() { }

private:
  virtual REFCLSID GetCLSID() { return CLSID_DirectSound8; }
  virtual DWORD GetCooperativeLevel() { return DSSCL_NORMAL; }

  // DirectSound 8 doesn't need a primary buffer
  virtual bool CreatePrimarySoundBuffer(IDirectSound*) { return true; }
};


#endif
