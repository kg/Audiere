#ifndef OUTPUT_NULL_HPP
#define OUTPUT_NULL_HPP


#include <set>
#include "audiere.h"
#include "types.h"


namespace audiere {

  class NullOutputStream;

  class NullAudioDevice : public DLLImplementation<AudioDevice> {
  public:
    NullAudioDevice();
    ~NullAudioDevice();

    bool initialize(const char* parameters);
    void update();
    OutputStream* openStream(SampleSource* source);

  private:
    void removeStream(NullOutputStream* stream);

    typedef std::set<NullOutputStream*> StreamList;
    StreamList m_streams;

    friend class NullOutputStream;
  };

  class NullOutputStream : public DLLImplementation<OutputStream> {
  private:
    NullOutputStream(NullAudioDevice* device, SampleSource* source);
    ~NullOutputStream();

  public:
    void play();
    void stop();
    void reset();
    bool isPlaying();
    void setVolume(int volume);
    int  getVolume();

  private:
    void resetTimer();
    void update();
    int dummyRead(int samples_to_read);

    NullAudioDevice* m_device;
                          
    SampleSource* m_source;
    int m_channel_count;    //
    int m_sample_rate;      // cached stream format
    int m_bits_per_sample;  //

    bool m_is_playing;
    int m_volume;

    u64 m_last_update;

    friend class NullAudioDevice;
  };

}


#endif
