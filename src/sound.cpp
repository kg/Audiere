#include "audiere.h"
#include "internal.h"
#include "repeatable.h"
#include "utility.h"


namespace audiere {

  OutputStream* OpenBuffer(
    AudioDevice* device, SampleSource* source, bool first);

  OutputStream* OpenStream(
    AudioDevice* device,
    SampleSource* source,
    bool first)
  {
    // if the device does not support streaming, try to open the sound as
    // buffered audio
    if (!device->supportsStreaming()) {
      return (first ? OpenBuffer(device, source, false) : 0);
    }

    return device->openStream(source);
  }

  OutputStream* OpenBuffer(
    AudioDevice* device,
    SampleSource* source,
    bool first)
  {
    // if the stream is not seekable, we cannot know how big of a buffer
    // to allocate, so try to stream it
    if (!source->isSeekable()) {
      return (first ? OpenStream(device, source, false) : 0);
    }

    int stream_length = source->getLength();
    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    int stream_length_bytes =
      stream_length * channel_count * GetBytesPerSample(sample_format);
    u8* buffer = new u8[stream_length_bytes];
    source->setPosition(0);
    source->read(stream_length, buffer);

    /// @todo  open the output buffer with these samples and return that sound
    delete[] buffer;
    return 0;
  }

  ADR_EXPORT(OutputStream*, AdrOpenSound)(
    AudioDevice* device,
    SampleSource* source,
    SoundMode mode)
  {
    if (!device || !source) {
      return 0;
    }

    switch (mode) {
      case STREAM: return OpenStream(device, source, true);
      case BUFFER: return OpenBuffer(device, source, true);
      default:     return 0;
    }
  }

}
