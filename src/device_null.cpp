#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <algorithm>
#include <functional>
#include "device_null.h"
#include "timer.h"
#include "threads.h"
#include "utility.h"

namespace audiere {

  NullAudioDevice::NullAudioDevice() {
  }


  NullAudioDevice::~NullAudioDevice() {
    ADR_ASSERT(m_streams.size() == 0,
      "Null output context should not die with open streams");
  }


  bool
  NullAudioDevice::initialize(const char* /*parameters*/) {
    return true;
  }


  void
  NullAudioDevice::update() {
    /*
      UGH.  We can't use VC6 here because you can't use mem_fun with void
      functions.  :(
    std::for_each(
      m_streams.begin(),
      m_streams.end(),
      std::mem_fun(&NullOutputStream::Update));
    */

    StreamList::iterator i = m_streams.begin();
    for (; i != m_streams.end(); ++i) {
      (*i)->update();
    }

    AI_Sleep(50);
  }


  OutputStream*
  NullAudioDevice::openStream(SampleSource* source) {
    NullOutputStream* stream = new NullOutputStream(this, source);
    m_streams.insert(stream);
    return stream;
  }


  void
  NullAudioDevice::removeStream(NullOutputStream* stream) {
    m_streams.erase(stream);
  }


  NullOutputStream::NullOutputStream(
    NullAudioDevice* device,
    SampleSource* source)
  : m_device(device)
  , m_source(source)
  , m_is_playing(false)
  , m_volume(MaximumVolume)
  , m_last_update(0)
  {
    m_device->ref();
    m_source->getFormat(m_channel_count, m_sample_rate, m_bits_per_sample);
  }


  NullOutputStream::~NullOutputStream() {
    m_device->removeStream(this);
    m_device->unref();

    delete m_source;
  }


  void
  NullOutputStream::play() {
    ADR_GUARD("NullOutputStream::Play");
    m_is_playing = true;
    resetTimer();
  }


  void
  NullOutputStream::stop() {
    m_is_playing = false;
  }


  void
  NullOutputStream::reset() {
    resetTimer();
  }


  bool
  NullOutputStream::isPlaying() {
    return m_is_playing;
  }


  void
  NullOutputStream::setVolume(int volume) {
    m_volume = volume;
  }


  int
  NullOutputStream::getVolume() {
    return m_volume;
  }


  void
  NullOutputStream::resetTimer() {
    m_last_update = GetNow();
  }


  void
  NullOutputStream::update() {
    ADR_GUARD("NullOutputStream::Update");

    if (m_is_playing) {
      ADR_LOG("Null output stream is playing");

      // get number of milliseconds elapsed since last playing update
      // so we can read that much time worth of samples
      u64 now = GetNow();
      u64 elapsed = now - m_last_update;

      ADR_IF_DEBUG {
        char str[100];
        sprintf(str, "Elapsed: %I64d", elapsed);
        ADR_LOG(str);
      }

      int samples_to_read = int(m_sample_rate * elapsed / 1000000);

      ADR_IF_DEBUG {
        char str[100];
        sprintf(str, "Samples to read: %d", samples_to_read);
        ADR_LOG(str);
      }

      int samples_read = dummyRead(samples_to_read);

      if (samples_read != samples_to_read) {
        ADR_LOG("Stopping null output stream");
        m_source->reset();
        stop();
      }

      m_last_update = now;
    }
  }


  int
  NullOutputStream::dummyRead(int samples_to_read) {
    int total = 0;  // number of samples read so far

    // read samples into dummy buffer, counting the number we actually read
    u8* dummy = new u8[1024 * m_channel_count * m_bits_per_sample / 8];
    while (samples_to_read > 0) {
      int read = Min(1024, samples_to_read);
      int actual_read = m_source->read(read, dummy);
      total += actual_read;
      samples_to_read -= actual_read;
      if (actual_read < read) {
        break;
      }
    }

    delete[] dummy;
    return total;
  }

}
