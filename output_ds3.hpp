#ifndef OUTPUT_DS3_HPP
#define OUTPUT_DS3_HPP


#include "output_ds.hpp"


class DS3OutputStream;


class DS3OutputContext : public DSOutputContext
{
public:
  DS3OutputContext() { m_PrimaryBuffer = 0; }
  ~DS3OutputContext();

private:
  virtual REFCLSID GetCLSID() { return CLSID_DirectSound; };

  // DirectSound 3 needs to be able to call SetFormat() on the primary buffer
  virtual DWORD GetCooperativeLevel() { return DSSCL_PRIORITY; }

  virtual bool CreatePrimarySoundBuffer();

private:
  IDirectSoundBuffer* m_PrimaryBuffer;
};


#endif
