#include "device_mm.h"
#include "buffer_stream.h"


namespace audiere {

  MMAudioDevice*
  MMAudioDevice::create(ParameterList& parameters) {
    return 0;
  }


  MMAudioDevice::MMAudioDevice() {
    m_device = NULL;
  }


  MMAudioDevice::~MMAudioDevice() {
    
  }


  void
  MMAudioDevice::update() {
  }


  OutputStream*
  MMAudioDevice::openStream(SampleSource* source) {
    RefPtr<SampleSource> s(source);
    return 0;
  }


  OutputStream*
  MMAudioDevice::openBuffer(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(new BufferStream(
      samples, sample_count,
      channel_count, sample_rate, sample_format));
  }

}
