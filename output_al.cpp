#include "output_al.hpp"


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
      success = (alGetError() == AL_NO_ERROR);
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

  // should we process it too?
  alcProcessContext(m_Context);

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
ALOutputContext::OpenStream(
  int channel_count,
  int sample_rate,
  int bits_per_sample,
  ADR_SAMPLE_SOURCE source,
  ADR_SAMPLE_RESET reset,
  void* opaque)
{
  // generate buffers
  ALuint* buffers = new ALuint[channel_count * AL_BUFFER_COUNT];
  alGenBuffers(channel_count * AL_BUFFER_COUNT, buffers);
  if (alGetError() != AL_NO_ERROR) {
    delete[] buffers;
    return NULL;
  }

  // generate sources
  // we have one source for each channel
  ALuint* sources = new ALuint[channel_count];
  alGenSources(channel_count, sources);
  if (alGetError() != AL_NO_ERROR) {
    delete[] sources;
    alDeleteBuffers(channel_count * AL_BUFFER_COUNT, buffers);
    delete[] buffers;
    return NULL;
  }

  ALOutputStream* stream = new ALOutputStream(
    this,
    buffers,
    sources,
    channel_count,
    sample_rate,
    bits_per_sample);
  m_OpenStreams.push_back(stream);
  return stream;
}

////////////////////////////////////////////////////////////////////////////////

ALOutputStream::ALOutputStream(
  ALOutputContext* context,
  ALuint buffers[AL_BUFFER_COUNT],
  ALuint* sources,
  int channel_count,
  int sample_rate,
  int bits_per_sample)
{
  // fill the members
  m_Context       = context;
  m_ChannelCount  = channel_count;
  m_SampleRate    = sample_rate;
  m_BitsPerSample = bits_per_sample;

  memcpy(m_Buffers, buffers, sizeof(ALuint) * m_ChannelCount * AL_BUFFER_COUNT);
  memcpy(m_Sources, sources, sizeof(ALuint) * m_ChannelCount);

  m_IsPlaying = false;
  m_Volume    = ADR_VOLUME_MAX;
  m_Pan       = ADR_PAN_CENTER;

  // read samples and fill the buffers

  // initialize each source
  ALuint* buffer = m_Buffers;
  ALuint* source = m_Sources;
  int s = m_ChannelCount;
  while (s--) {
    // queue up this source's buffers
    alSourceQueueBuffers(*source, AL_BUFFER_COUNT, buffer);

    // make sure it's set to looping
    alSourcei(*source, AL_LOOPING, AL_TRUE);

    source++;
    buffer += AL_BUFFER_COUNT;
  }
}

////////////////////////////////////////////////////////////////////////////////

ALOutputStream::~ALOutputStream()
{
  alDeleteSources(m_ChannelCount, m_Sources);
  delete[] m_Sources;
  alDeleteBuffers(AL_BUFFER_COUNT * m_ChannelCount, m_Buffers);
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Update()
{
  // update the stream
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Play()
{
  alSourcePlayv(m_ChannelCount, m_Sources);
  m_IsPlaying = true;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Stop()
{
  alSourcePausev(m_ChannelCount, m_Sources);
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

  
}

////////////////////////////////////////////////////////////////////////////////

int
ALOutputStream::GetVolume()
{
  return m_Volume;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::SetPan(int pan)
{
  if (pan < ADR_PAN_LEFT) {
    pan = ADR_PAN_LEFT;
  } else if (pan > ADR_PAN_RIGHT) {
    pan = ADR_PAN_RIGHT;
  }

  m_Pan = pan;


}

////////////////////////////////////////////////////////////////////////////////

int
ALOutputStream::GetPan()
{
  return m_Pan;
}

////////////////////////////////////////////////////////////////////////////////
