#ifndef DEVICE_DS8_H
#define DEVICE_DS8_H


#include "device_ds.h"


namespace audiere {

  class DS8AudioDevice : public DSAudioDevice {
  private:
    REFCLSID getCLSID() {
      return CLSID_DirectSound8;
    }

    DWORD getCooperativeLevel() {
      return DSSCL_NORMAL;
    }

    // DirectSound 8 doesn't need a primary buffer
    bool createPrimarySoundBuffer(IDirectSound*) {
      return true;
    }
  };

}


#endif
