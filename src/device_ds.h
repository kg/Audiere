#ifndef DEVICE_DS_H
#define DEVICE_DS_H


// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
#include <list>
#include "audiere.h"
#include "internal.h"
#include "repeatable.h"
#include "threads.h"
#include "utility.h"


namespace audiere {

  class DSOutputStream;
  class DSOutputBuffer;

  class DSAudioDevice
    : public RefCountedImplementation<AudioDevice>
    , public Synchronized
  {
  public:
    DSAudioDevice();
    ~DSAudioDevice();

    bool initialize(ParameterList& parameters);
    void update();
    OutputStream* openStream(SampleSource* source);
    OutputStream* openBuffer(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format);

  private:
    typedef std::list<DSOutputStream*> StreamList;
    typedef std::list<DSOutputBuffer*> BufferList;

    void removeStream(DSOutputStream* stream);
    void removeBuffer(DSOutputBuffer* buffer);

    IDirectSound* m_direct_sound;
    StreamList    m_open_streams;
    BufferList    m_open_buffers;

    /// length of streaming buffer in milliseconds
    int m_buffer_length;

    HWND m_anonymous_window;


    // these must be overridden
    virtual REFCLSID getCLSID() = 0;
    virtual DWORD getCooperativeLevel() = 0;
    virtual bool createPrimarySoundBuffer(IDirectSound* ds) = 0;

    friend class DSOutputStream;
  };


  class DSOutputStream : public DLLImplementation<OutputStream> {
  private:
    DSOutputStream(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      int buffer_length, // in samples
      SampleSource* source);
    ~DSOutputStream();

  public:
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
    DSAudioDevice* m_device;

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


  class DSOutputBuffer : public DLLImplementation<SampleSource> {
  private:
/*
    DSOutputStream(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      void* samples, int sample_count,
*/
  public:
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
    
  };

}


#endif
