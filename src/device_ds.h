#ifndef DEVICE_DS_HPP
#define DEVICE_DS_HPP


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


namespace audiere {

  class DSOutputStream;

  class DSAudioDevice
    : public RefCountedImplementation<AudioDevice>
    , public Synchronized
  {
  public:
    DSAudioDevice();
    ~DSAudioDevice();

    bool initialize(const char* parameters);
    bool supportsStreaming();
    void update();
    OutputStream* openStream(SampleSource* source);

  private:
    typedef std::list<DSOutputStream*> StreamList;

    void removeStream(DSOutputStream* stream);

    IDirectSound* m_direct_sound;
    StreamList    m_open_streams;

    int m_buffer_length;  // in milliseconds

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
      int sample_size,   // in bytes
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
  
    // returns true if |position| is between |start| (inclusive) and
    // |end| (exclusive) in the buffer
    bool isBetween(int position, int start, int end);

  private:
    DSAudioDevice* m_device;

    IDirectSoundBuffer* m_buffer;
    int m_buffer_length;  // in samples
    int m_next_read;  // offset (in samples) where we will read next

    RepeatableStream* m_source;
    int m_sample_size;  // convenience: bits per sample * channel count / 8

    float m_volume;
    float m_pan;
    ::BYTE* m_last_sample; // the last sample read (used for clickless silence)

    friend class DSAudioDevice;
  };

}


#endif
