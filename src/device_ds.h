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
#include "device_ds_stream.h"
#include "internal.h"
#include "threads.h"
#include "utility.h"


namespace audiere {

  class DSOutputStream;
  class DSOutputBuffer;

  class DSAudioDevice
    : public RefImplementation<AudioDevice>
    , public Synchronized
  {
  public:
    static DSAudioDevice* create(const ParameterList& parameters);

  private:
    DSAudioDevice(
      int buffer_length,
      HWND anonymous_window,
      IDirectSound* direct_sound);
    ~DSAudioDevice();

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

    friend class DSOutputStream;
  };


}


#endif
