#ifndef DEVICE_MM_H
#define DEVICE_MM_H


#include <windows.h>
#include <mmsystem.h>
#include <queue>
#include "internal.h"
#include "mixer.h"
#include "threads.h"
#include "utility.h"


namespace audiere {

  class MMAudioDevice
    : public RefImplementation<AudioDevice>
    , public Synchronized
  {
  public:
    static MMAudioDevice* create(const ParameterList& parameters);

  private:
    MMAudioDevice(HWAVEOUT device);
    ~MMAudioDevice();

  public:
    void ADR_CALL update();
    OutputStream* ADR_CALL openStream(SampleSource* source);
    OutputStream* ADR_CALL openBuffer(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format);

  private:
    // 4 buffers of 1000 frames is 4000 frames at 44100 Hz is about
    // 91 milliseconds of audio
    enum {
      BUFFER_COUNT  = 4,
      BUFFER_LENGTH = 1000 * 4, // 1000 samples, 4000 bytes
    };

    HWAVEOUT m_device;
    RefPtr<Mixer> m_mixer;

    int m_current_buffer;
    WAVEHDR m_buffers[BUFFER_COUNT];
    u8 m_samples[BUFFER_COUNT * BUFFER_LENGTH];

    friend class MMOutputStream;
  };


  class MMOutputStream : public RefImplementation<OutputStream> {
  private:
    MMOutputStream(MMAudioDevice* device, SampleSource* source);
    ~MMOutputStream();

  public:
    void ADR_CALL play();
    void ADR_CALL stop();
    bool ADR_CALL isPlaying();
    void ADR_CALL reset();

    void  ADR_CALL setRepeat(bool repeat);
    bool  ADR_CALL getRepeat();
    void  ADR_CALL setVolume(float volume);
    float ADR_CALL getVolume();
    void  ADR_CALL setPan(float pan);
    float ADR_CALL getPan();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    Mixer& getMixer();

    RefPtr<MMAudioDevice> m_device;
    SampleSource* m_source;
    float volume;
    
    friend class MMAudioDevice;
  };

}


#endif
