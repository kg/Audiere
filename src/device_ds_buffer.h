#ifndef DEVICE_DS_BUFFER_H
#define DEVICE_DS_BUFFER_H


#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
#include "audiere.h"


namespace audiere {

  class DSAudioDevice;

  class DSOutputBuffer : public RefImplementation<OutputStream> {
  public:
    DSOutputBuffer(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      int length,
      int frame_size);
    ~DSOutputBuffer();

    void ADR_CALL play();
    void ADR_CALL stop();
    bool ADR_CALL isPlaying();
    void ADR_CALL reset();

    void ADR_CALL setRepeat(bool repeat);
    bool ADR_CALL getRepeat();

    void  ADR_CALL setVolume(float volume);
    float ADR_CALL getVolume();

    void  ADR_CALL setPan(float pan);
    float ADR_CALL getPan();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    RefPtr<DSAudioDevice> m_device;
    IDirectSoundBuffer* m_buffer;
    int m_length;
    int m_frame_size;

    bool  m_repeating;
    float m_volume;
    float m_pan;
  };

}


#endif
