#ifndef DEVICE_DS3_H
#define DEVICE_DS3_H


#include "device_ds.h"


namespace audiere {

  class DS3AudioDevice : public DSAudioDevice {
  public:
    DS3AudioDevice();
    ~DS3AudioDevice();

  private:
    REFCLSID getCLSID() {
      return CLSID_DirectSound;
    };

    // DirectSound 3 needs to be able to call SetFormat() on the primary buffer
    DWORD getCooperativeLevel() {
      return DSSCL_PRIORITY;
    }

    bool createPrimarySoundBuffer(IDirectSound* ds);

  private:
    IDirectSoundBuffer* m_PrimaryBuffer;
  };

}


#endif
