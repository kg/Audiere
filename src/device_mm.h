#ifndef DEVICE_MM_H
#define DEVICE_MM_H


#include <windows.h>
#include <mmsystem.h>
#include <list>
#include "internal.h"
#include "threads.h"
#include "utility.h"


namespace audiere {

  class MMOutputStream;


  class MMAudioDevice
    : public RefImplementation<AudioDevice>
    , public Synchronized
  {
  public:
    static MMAudioDevice* create(ParameterList& parameters);

  private:
    MMAudioDevice();
    ~MMAudioDevice();

  public:
    void update();
    OutputStream* openStream(SampleSource* source);
    OutputStream* openBuffer(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format);

  private:
    typedef std::list<MMOutputStream*> StreamList;

    void removeStream(MMOutputStream* stream);

  private:
    HWAVEOUT m_device;
    StreamList m_open_streams;
  };

}


#endif
