#include <vector>
#include "internal.h"


namespace audiere {

  class SingleSoundEffect : public RefImplementation<SoundEffect> {
  public:
    SingleSoundEffect(OutputStream* os) {
      m_stream = os;
    }

    void ADR_CALL play() {
      if (m_stream->isPlaying()) {
        m_stream->reset();
        m_stream->play();
      } else {
        m_stream->reset();
        m_stream->play();
      }
    }

    void ADR_CALL stop() {
      m_stream->stop();
      m_stream->reset();
    }

  private:
    RefPtr<OutputStream> m_stream;
  };


  class MultipleSoundEffect : public RefImplementation<SoundEffect> {
  public:
    MultipleSoundEffect(AudioDevice* device, SampleBuffer* sb) {
      m_device = device;
      m_buffer = sb;
    }

    void ADR_CALL play() {
      // open the stream and play it
      OutputStream* stream = m_device->openStream(m_buffer->openStream());
      if (!stream) {
        return;
      }
      stream->play();

      // go through the list and see if any streams are done playing
      // so we can reuse them
      for (unsigned i = 0; i < m_streams.size(); ++i) {
        if (!m_streams[i]->isPlaying()) {
          m_streams[i] = stream;
          return;
        }
      }

      m_streams.push_back(stream);
    }

    void ADR_CALL stop() {
      m_streams.clear();
    }

  private:
    RefPtr<AudioDevice> m_device;
    RefPtr<SampleBuffer> m_buffer;
    std::vector<RefPtr<OutputStream> > m_streams;
  };


  ADR_EXPORT(SoundEffect*, AdrOpenSoundEffect)(
    AudioDevice* device,
    SampleSource* source,
    SoundEffectType type)
  {
    if (!device || !source) {
      return 0;
    }

    switch (type) {
      case SINGLE: {
        OutputStream* os = OpenSound(device, source, true);
        return (os ? new SingleSoundEffect(os) : 0);
      }
        
      case MULTIPLE: {
        SampleBuffer* sb = CreateSampleBuffer(source);
        return (sb ? new MultipleSoundEffect(device, sb) : 0);
      }

      default:
        return 0;
    }
  }

}
