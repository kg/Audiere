#include <limits.h>
#include "output_al.hpp"


static const int BUFFER_COUNT = 4;
static const int BUFFER_MILLISECONDS = 250;


////////////////////////////////////////////////////////////////////////////////

ALOutputContext::ALOutputContext()
{
  m_Device = NULL;
  m_Context = NULL;
}

////////////////////////////////////////////////////////////////////////////////

ALOutputContext::~ALOutputContext()
{
  if (m_Context) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_Context);
    m_Context = NULL;
  }

  if (m_Device) {
    alcCloseDevice(m_Device);
    m_Device = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
ALOutputContext::Initialize(const char* parameters)
{
  // are we already initialized?
  if (m_Device) {
    return false;
  }

  // open an output device
  m_Device = alcOpenDevice(NULL);
  if (!m_Device) {
    return false;
  }

  // create a rendering context
  m_Context = alcCreateContext(m_Device, NULL);
  if (!m_Context) {
    alcCloseDevice(m_Device);
    m_Device = NULL;
    return false;
  }

  alcMakeContextCurrent(m_Context);

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
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_Context);
    m_Context = NULL;
    alcCloseDevice(m_Device);
    m_Device = NULL;
    return false;
  }

  return (m_Device != NULL);
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputContext::Update()
{
  // enumerate all open streams
  StreamList::iterator i = m_OpenStreams.begin();
  while (i != m_OpenStreams.end()) {
    ALOutputStream* s = *i++;
    s->Update();
  }
}

////////////////////////////////////////////////////////////////////////////////

IOutputStream*
ALOutputContext::OpenStream(ISampleSource* source)
{
  int channel_count;
  int sample_rate;
  int bits_per_sample;
  source->GetFormat(channel_count, sample_rate, bits_per_sample);

  // calculate OpenAL format
  ALenum format;
  if (channel_count == 1 && bits_per_sample == 8) {
    format = AL_FORMAT_MONO8;
  } else if (channel_count == 1 && bits_per_sample == 16) {
    format = AL_FORMAT_MONO16;
  } else if (channel_count == 2 && bits_per_sample == 8) {
    format = AL_FORMAT_STEREO8;
  } else if (channel_count == 2 && bits_per_sample == 16) {
    format = AL_FORMAT_STEREO16;
  } else {
    return NULL;
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
  m_OpenStreams.push_back(stream);
  return stream;
}

////////////////////////////////////////////////////////////////////////////////

ALOutputStream::ALOutputStream(
  ALOutputContext* context,
  ISampleSource* source,
  ALuint al_source,
  ALuint* buffers,
  ALenum format,
  int sample_rate)
{
  // fill the members
  m_Context = context;
  m_Source = source;

  m_SampleRate = sample_rate;
  m_Format = format;
  switch (format) {
    case AL_FORMAT_MONO8:    m_SampleSize = 1; break;
    case AL_FORMAT_MONO16:   m_SampleSize = 2; break;
    case AL_FORMAT_STEREO8:  m_SampleSize = 2; break;
    case AL_FORMAT_STEREO16: m_SampleSize = 4; break;
  }

  m_LastSample = new ALubyte[m_SampleSize];
  memset(m_LastSample, 0, m_SampleSize);

  m_ALSource = al_source;
  m_Buffers  = buffers;

  m_IsPlaying = false;
  m_Volume    = ADR_VOLUME_MAX;

  // calculate the desired length (in samples) of each buffer
  m_BufferLength = BUFFER_MILLISECONDS * m_SampleRate / 1000;

  FillBuffers();

  // queue up the source's buffers
  alSourceQueueBuffers(m_ALSource, BUFFER_COUNT, m_Buffers);
}

////////////////////////////////////////////////////////////////////////////////

ALOutputStream::~ALOutputStream()
{
  // remove ourself from the list
  ALOutputContext::StreamList::iterator i = m_Context->m_OpenStreams.begin();
  while (i != m_Context->m_OpenStreams.end()) {
    if (*i == this) {
      m_Context->m_OpenStreams.erase(i);
      break;
    }
    ++i;
  }

  alDeleteSources(1, &m_ALSource);
  alDeleteBuffers(BUFFER_COUNT, m_Buffers);
  delete[] m_Buffers;
  delete[] m_LastSample;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Update()
{
  // are there any buffers that have been processed?
  ALint processed_buffers;
#ifdef _WIN32 // XXX more OpenAL hacks
  alGetSourcei(m_ALSource, AL_BUFFERS_PROCESSED, &processed_buffers);
#else
  alGetSourceiv(m_ALSource, AL_BUFFERS_PROCESSED, &processed_buffers);
#endif

  // don't do any allocations if we don't need to
  if (processed_buffers == 0) {
    return;
  }

  int read_size_samples = m_BufferLength;
  int read_size_bytes = read_size_samples * m_SampleSize;
  ALubyte* sample_buffer = new ALubyte[read_size_bytes];

  int buffer_length_bytes = read_size_bytes;
  while (processed_buffers--) {

    Read(sample_buffer, read_size_samples);

    // unqueue/refill/queue
    ALuint buffer;
    alSourceUnqueueBuffers(m_ALSource, 1, &buffer);

    alBufferData(
      buffer,
      m_Format,
      sample_buffer,
      buffer_length_bytes,
      m_SampleRate);

    alSourceQueueBuffers(m_ALSource, 1, &buffer);
  }

  delete[] sample_buffer;
}

////////////////////////////////////////////////////////////////////////////////

int
ALOutputStream::Read(void* samples, int sample_count)
{
  // try to read from the stream
  int samples_read = m_Source->Read(sample_count, samples);

  // read the last sample
  if (samples_read > 0) {
    memcpy(
      m_LastSample,
      (ALubyte*)samples + (samples_read - 1) * m_SampleSize,
      m_SampleSize
    );
  }

  // fill the rest with silence
  ALubyte* out = (ALubyte*)samples + m_SampleSize * samples_read;
  int c = sample_count - samples_read;
  while (c--) {
    memcpy(out, m_LastSample, m_SampleSize);
    out += m_SampleSize;
  }

  return samples_read;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::FillBuffers()
{
  int samples_to_read = m_BufferLength * BUFFER_COUNT;
  int allocate = samples_to_read * m_SampleSize;
  ALubyte* sample_buffer = new ALubyte[allocate];
  Read(sample_buffer, samples_to_read);
   
  // stick the data into the buffers
  ALubyte* samples = sample_buffer;
  ALuint*  buffer  = m_Buffers;

  for (int i = 0; i < BUFFER_COUNT; i++) {
      
    alBufferData(
      *buffer,
      m_Format,
      samples,
      m_BufferLength * m_SampleSize,
      m_SampleRate
    );

    samples += m_BufferLength * m_SampleSize;
    buffer++;
  }

  delete[] sample_buffer;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Play()
{
  alSourcePlay(m_ALSource);
  m_IsPlaying = true;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Stop()
{
  alSourcePause(m_ALSource);
  m_IsPlaying = false;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Reset()
{
  bool is_playing = IsPlaying();
  if (is_playing) {
    Stop();
  }

  // reset stream

  if (is_playing) {
    Play();
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
ALOutputStream::IsPlaying()
{
  return m_IsPlaying;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::SetVolume(int volume)
{
  if (volume < ADR_VOLUME_MIN) {
    volume = ADR_VOLUME_MIN;
  } else if (volume > ADR_VOLUME_MAX) {
    volume = ADR_VOLUME_MAX;
  }

  m_Volume = volume;

  float v = (float)m_Volume / ADR_VOLUME_MAX;
  alSourcef(m_ALSource, AL_GAIN, v);
}

////////////////////////////////////////////////////////////////////////////////

int
ALOutputStream::GetVolume()
{
  return m_Volume;
}

////////////////////////////////////////////////////////////////////////////////
