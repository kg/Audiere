#include "device_oal.h"
#include "threads.h"


static const int BUFFER_COUNT = 4;
static const int BUFFER_MILLISECONDS = 250;


namespace audiere {

  OALAudioDevice*
  OALAudioDevice::create(const ParameterList& parameters) {
    // open an output device
    ALCdevice* device = alcOpenDevice(0);
    if (!device) {
      return 0;
    }

    // create a rendering context
    ALCcontext* context = alcCreateContext(device, 0);
    if (!context) {
      alcCloseDevice(device);
      return false;
    }

    alcMakeContextCurrent(context);

    // define the listener state
    ALfloat position[]    = { 0.0, 0.0, 0.0 };
    ALfloat velocity[]    = { 0.0, 0.0, 0.0 };
    ALfloat orientation[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };

    // set the listener state
    bool success = false;
    alListenerfv(AL_POSITION, position);
    if (alGetError() == AL_NO_ERROR) {
      alListenerfv(AL_VELOCITY, velocity);
      if (alGetError() == AL_NO_ERROR) {
        alListenerfv(AL_ORIENTATION, orientation);
        if (alGetError() == AL_NO_ERROR) {

          // set the distance model
          alDistanceModel(AL_NONE);
          success = (alGetError() == AL_NO_ERROR);

        }
      }
    }

    // if we failed, go home
    if (!success) {
      alcMakeContextCurrent(0);
      alcDestroyContext(context);
      alcCloseDevice(device);
      return 0;
    }

    return new OALAudioDevice(device, context);
  }


  OALAudioDevice::OALAudioDevice(ALCdevice* device, ALCcontext* context) {
    m_device  = device;
    m_context = context;
  }


  OALAudioDevice::~OALAudioDevice() {
    if (m_context) {
      alcMakeContextCurrent(0);
      alcDestroyContext(m_context);
      m_context = 0;
    }

    if (m_device) {
      alcCloseDevice(m_device);
      m_device = 0;
    }
  }


  void
  OALAudioDevice::update() {
    // enumerate all open streams
    StreamList::iterator i = m_open_streams.begin();
    while (i != m_open_streams.end()) {
      OALOutputStream* s = *i++;
      s->update();
    }
  }


  OutputStream*
  OALAudioDevice::openStream(SampleSource* source) {
    if (!source) {
      return 0;
    }

    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    // calculate OpenAL format
    ALenum format;
    if (channel_count == 1 && sample_format == SF_U8) {
      format = AL_FORMAT_MONO8;
    } else if (channel_count == 1 && sample_format == SF_S16) {
      format = AL_FORMAT_MONO16;
    } else if (channel_count == 2 && sample_format == SF_U8) {
      format = AL_FORMAT_STEREO8;
    } else if (channel_count == 2 && sample_format == SF_S16) {
      format = AL_FORMAT_STEREO16;
    } else {
      return 0;
    }

    // generate buffers
    ALuint* buffers = new ALuint[BUFFER_COUNT];
    alGenBuffers(BUFFER_COUNT, buffers);
    if (alGetError() != AL_NO_ERROR) {
      delete[] buffers;
      return NULL;
    }

    // generate sources
    // we have one source for each channel
    ALuint al_source;
    alGenSources(1, &al_source);
    if (alGetError() != AL_NO_ERROR) {
      alDeleteBuffers(BUFFER_COUNT, buffers);
      delete[] buffers;
      return NULL;
    }

    OALOutputStream* stream = new OALOutputStream(
      this,
      source,
      al_source,
      buffers,
      format,
      sample_rate);
    m_open_streams.push_back(stream);
    return stream;
  }


  OutputStream*
  OALAudioDevice::openBuffer(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    /// @todo  implement ALAudioDevice::openBuffer
    return 0;
  }


  void
  OALAudioDevice::removeStream(OALOutputStream* stream) {
    m_open_streams.remove(stream);
  }


  OALOutputStream::OALOutputStream(
    OALAudioDevice* device,
    SampleSource* source,
    ALuint al_source,
    ALuint* buffers,
    ALenum format,
    int sample_rate)
  {
    m_device = device;

    m_source = source;

    m_sample_rate = sample_rate;
    m_format = format;
    switch (format) {
      case AL_FORMAT_MONO8:    m_sample_size = 1; break;
      case AL_FORMAT_MONO16:   m_sample_size = 2; break;
      case AL_FORMAT_STEREO8:  m_sample_size = 2; break;
      case AL_FORMAT_STEREO16: m_sample_size = 4; break;
    }

    m_last_sample = new ALubyte[m_sample_size];
    memset(m_last_sample, 0, m_sample_size);

    m_ALsource = al_source;
    m_buffers  = buffers;

    m_is_playing = false;
    m_volume     = 1;

    // calculate the desired length (in samples) of each buffer
    m_buffer_length = BUFFER_MILLISECONDS * m_sample_rate / 1000;

    fillBuffers();

    // queue up the source's buffers
    alSourceQueueBuffers(m_ALsource, BUFFER_COUNT, m_buffers);
  }


  OALOutputStream::~OALOutputStream() {
    m_device->removeStream(this);

    alDeleteSources(1, &m_ALsource);
    alDeleteBuffers(BUFFER_COUNT, m_buffers);
    delete[] m_buffers;
    delete[] m_last_sample;
  }


  void
  OALOutputStream::update() {
    // are there any buffers that have been processed?
    ALint processed_buffers;
#ifdef _WIN32 // XXX more OpenAL hacks
    alGetSourcei(m_ALsource, AL_BUFFERS_PROCESSED, &processed_buffers);
#else
    alGetSourceiv(m_ALsource, AL_BUFFERS_PROCESSED, &processed_buffers);
#endif

    // don't do any allocations if we don't need to
    if (processed_buffers == 0) {
      return;
    }

    int read_size_samples = m_buffer_length;
    int read_size_bytes = read_size_samples * m_sample_size;
    ALubyte* sample_buffer = new ALubyte[read_size_bytes];

    int buffer_length_bytes = read_size_bytes;
    while (processed_buffers--) {

      if (read(sample_buffer, read_size_samples) == 0) {
        stop();
        break;
      }

      // unqueue/refill/queue
      ALuint buffer;
      alSourceUnqueueBuffers(m_ALsource, 1, &buffer);

      alBufferData(
        buffer,
        m_format,
        sample_buffer,
        buffer_length_bytes,
        m_sample_rate);

      alSourceQueueBuffers(m_ALsource, 1, &buffer);
    }

    delete[] sample_buffer;

    AI_Sleep(50);
  }


  int
  OALOutputStream::read(void* samples, int sample_count) {
    // try to read from the stream
    int samples_read = m_source->read(sample_count, samples);

    // read the last sample
    if (samples_read > 0) {
      memcpy(
        m_last_sample,
        (ALubyte*)samples + (samples_read - 1) * m_sample_size,
        m_sample_size);
    }

    // fill the rest with silence
    ALubyte* out = (ALubyte*)samples + m_sample_size * samples_read;
    int c = sample_count - samples_read;
    while (c--) {
      memcpy(out, m_last_sample, m_sample_size);
      out += m_sample_size;
    }

    return samples_read;
  }


  void
  OALOutputStream::fillBuffers() {
    int samples_to_read = m_buffer_length * BUFFER_COUNT;
    int allocate = samples_to_read * m_sample_size;
    ALubyte* sample_buffer = new ALubyte[allocate];
    read(sample_buffer, samples_to_read);
   
    for (int i = 0; i < BUFFER_COUNT; i++) {      
      alBufferData(
        m_buffers[i],
        m_format,
        sample_buffer + i * m_buffer_length * m_sample_size,
        m_buffer_length * m_sample_size,
        m_sample_rate);
    }

    delete[] sample_buffer;
  }


  void
  OALOutputStream::play() {
    alSourcePlay(m_ALsource);
    m_is_playing = true;
  }


  void
  OALOutputStream::stop() {
    alSourcePause(m_ALsource);
    m_is_playing = false;
  }


  bool
  OALOutputStream::isPlaying() {
    return m_is_playing;
  }


  void
  OALOutputStream::reset() {
    bool is_playing = isPlaying();
    if (is_playing) {
      stop();
    }

    // reset stream

    if (is_playing) {
      play();
    }
  }


  void
  OALOutputStream::setRepeat(bool repeat) {
    /// @todo  implement ALOutputStream::setRepeat
  }


  bool
  OALOutputStream::getRepeat() {
    /// @todo  implement ALOutputStream::getRepeat
    return false;
  }


  void
  OALOutputStream::setVolume(float volume) {
    m_volume = volume;
    alSourcef(m_ALsource, AL_GAIN, volume);
  }


  float
  OALOutputStream::getVolume() {
    return m_volume;
  }


  void
  OALOutputStream::setPan(float pan) {
    /// @todo  implement ALOutputStream::setPan
  }


  float
  OALOutputStream::getPan() {
    /// @todo  implement ALOutputStream::getPan
    return 0.0f;
  }


  bool
  OALOutputStream::isSeekable() {
    /// @todo  implement ALOutputStream::isSeekable
    return false;
  }


  int
  OALOutputStream::getLength() {
    /// @todo  implement ALOutputStream::getLength
    return 0;
  }


  void
  OALOutputStream::setPosition(int /*position*/) {
    /// @todo  implement ALOutputStream::setPosition
  }


  int
  OALOutputStream::getPosition() {
    /// @todo  implement ALOutputStream::getPosition
    return 0;
  }

}
