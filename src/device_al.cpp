#include "device_al.h"
#include "threads.h"


static const int BUFFER_COUNT = 4;
static const int BUFFER_MILLISECONDS = 250;


namespace audiere {

  ALAudioDevice::ALAudioDevice() {
    m_device = 0;
    m_context = 0;
  }


  ALAudioDevice::~ALAudioDevice() {
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


  bool
  ALAudioDevice::initialize(const char* parameters) {
    // open an output device
    m_device = alcOpenDevice(0);
    if (!m_device) {
      return false;
    }

    // create a rendering context
    m_context = alcCreateContext(m_device, 0);
    if (!m_context) {
      alcCloseDevice(m_device);
      m_device = 0;
      return false;
    }

    alcMakeContextCurrent(m_context);

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
      alcDestroyContext(m_context);
      m_context = 0;
      alcCloseDevice(m_device);
      m_device = 0;
      return false;
    }

    return (m_device != 0);
  }


  void
  ALAudioDevice::update() {
    // enumerate all open streams
    StreamList::iterator i = m_open_streams.begin();
    while (i != m_open_streams.end()) {
      ALOutputStream* s = *i++;
      s->update();
    }
  }


  OutputStream*
  ALAudioDevice::openStream(SampleSource* source) {
    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    // calculate OpenAL format
    ALenum format;
    if (channel_count == 1 && sample_format == SF_U8) {
      format = AL_FORMAT_MONO8;
    } else if (channel_count == 1 && sample_format == SF_S16_LE) {
      format = AL_FORMAT_MONO16;
    } else if (channel_count == 2 && sample_format == SF_U8) {
      format = AL_FORMAT_STEREO8;
    } else if (channel_count == 2 && sample_format == SF_S16_LE) {
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

    ALOutputStream* stream = new ALOutputStream(
      this,
      source,
      al_source,
      buffers,
      format,
      sample_rate);
    m_open_streams.push_back(stream);
    return stream;
  }


  void
  ALAudioDevice::removeStream(ALOutputStream* stream) {
    // remove ourself from the list
    m_open_streams.remove(stream);
  }


  ALOutputStream::ALOutputStream(
    ALAudioDevice* device,
    SampleSource* source,
    ALuint al_source,
    ALuint* buffers,
    ALenum format,
    int sample_rate)
  {
    m_device = device;
    m_device->ref();

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


  ALOutputStream::~ALOutputStream() {
    m_device->removeStream(this);
    m_device->unref();

    alDeleteSources(1, &m_ALsource);
    alDeleteBuffers(BUFFER_COUNT, m_buffers);
    delete[] m_buffers;
    delete[] m_last_sample;

    delete m_source;
  }


  void
  ALOutputStream::update() {
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
  ALOutputStream::read(void* samples, int sample_count) {
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
  ALOutputStream::fillBuffers() {
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
  ALOutputStream::play() {
    alSourcePlay(m_ALsource);
    m_is_playing = true;
  }


  void
  ALOutputStream::stop() {
    alSourcePause(m_ALsource);
    m_is_playing = false;
  }


  void
  ALOutputStream::reset() {
    bool is_playing = isPlaying();
    if (is_playing) {
      stop();
    }

    // reset stream

    if (is_playing) {
      play();
    }
  }


  bool
  ALOutputStream::isPlaying() {
    return m_is_playing;
  }


  void
  ALOutputStream::setVolume(float volume) {
    m_volume = volume;
    alSourcef(m_ALsource, AL_GAIN, volume);
  }


  float
  ALOutputStream::getVolume() {
    return m_volume;
  }

}
