#ifndef DEVICE_DS_STREAM_H
#define DEVICE_DS_STREAM_H


#include "audiere.h"
#include "repeatable.h"


namespace audiere {

  class DSAudioDevice;

  class DSOutputStream : public RefImplementation<OutputStream> {
    DSOutputStream(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      int buffer_length, // in frames
      SampleSource* source);
    ~DSOutputStream();

    void play();
    void stop();
    bool isPlaying();
    void reset();

    void setRepeat(bool repeat);
    bool getRepeat();

    void  setVolume(float volume);
    float getVolume();

    void  setPan(float pan);
    float getPan();

    bool isSeekable();
    int  getLength();
    void setPosition(int position);
    int  getPosition();

  private:
    void fillStream();
    void update();
    int streamRead(int samples_to_read, void* buffer);
  
  private:
    RefPtr<DSAudioDevice> m_device;

    IDirectSoundBuffer* m_buffer;
    int m_buffer_length;  // in samples
    int m_next_read;  // offset (in samples) where we will read next
    int m_last_play;  // offset (in samples) where the play cursor was

    RepeatableStream* m_source;
    int m_sample_size;  // convenience: bytes per sample * channel count

    int m_total_read;    // total number of samples read from the stream
    int m_total_written; // total number of samples written 

    float m_volume;
    float m_pan;
    ::BYTE* m_last_sample; // the last sample read (used for clickless silence)

    friend class DSAudioDevice;
  };

}


#endif
