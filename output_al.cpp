// definitions:
// Most PCM data is stored with interleaved channels:
//   12121212
// A single sample for all channels ([12] in that case above) will be called
// a 'block', for lack of a better term.


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
ALOutputContext::OpenStream(
  int channel_count,
  int sample_rate,
  int bits_per_sample,
  ADR_SAMPLE_SOURCE source,
  ADR_SAMPLE_RESET reset,
  void* opaque)
{
  // OpenAL only supports 8-bit and 16-bit samples
  if (bits_per_sample != 8 && bits_per_sample != 16) {
    return NULL;
  }

  // generate buffers
  ALuint* buffers = new ALuint[channel_count * BUFFER_COUNT];
  alGenBuffers(channel_count * BUFFER_COUNT, buffers);
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
    alDeleteBuffers(channel_count * BUFFER_COUNT, buffers);
    delete[] buffers;
    return NULL;
  }

  ALOutputStream* stream = new ALOutputStream(
    this,
    source,
    reset,
    opaque,
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
  ADR_SAMPLE_SOURCE source,
  ADR_SAMPLE_RESET reset,
  void* opaque,
  ALuint buffers[BUFFER_COUNT],
  ALuint* sources,
  int channel_count,
  int sample_rate,
  int bits_per_sample)
{
  // fill the members
  m_Context = context;
  
  m_Source = source;
  m_Reset  = reset;
  m_Opaque = opaque;

  m_ChannelCount  = channel_count;
  m_SampleRate    = sample_rate;
  m_BitsPerSample = bits_per_sample;
  m_SampleSize    = m_BitsPerSample / 8;
  m_LastBlock     = new ALubyte[m_SampleSize * m_ChannelCount];
  memset(m_LastBlock, 0, m_SampleSize * m_ChannelCount);

  m_Buffers = buffers;
  m_Sources = sources;

  m_IsPlaying = false;
  m_Volume    = ADR_VOLUME_MAX;

  // calculate the desired length (in samples) of each buffer
  m_BufferLength = BUFFER_MILLISECONDS * m_SampleRate / 1000;

  FillBuffers();

  // initialize each source
  for (int s = 0; s < m_ChannelCount; s++) {

    // queue up this source's buffers
    alSourceQueueBuffers(
      m_Sources[s],
      BUFFER_COUNT,
      m_Buffers + s * BUFFER_COUNT
    );

    // position the source?
  }
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

  alDeleteSources(m_ChannelCount, m_Sources);
  delete[] m_Sources;
  alDeleteBuffers(BUFFER_COUNT * m_ChannelCount, m_Buffers);
  delete[] m_Buffers;
  delete[] m_LastBlock;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Update()
{
  // are there any buffers that have been processed?
  ALint processed_buffers = INT_MAX;
  for (int i = 0; i < 1/*m_ChannelCount*/; i++) {
    ALint buffers;
    alGetSourceiv(m_Sources[i], AL_BUFFERS_PROCESSED, &buffers);

    if (buffers < processed_buffers) {
      processed_buffers = buffers;
    }
  }

  // possibly bypass the rest of the processing
  if (processed_buffers == 0) {
    return;
  }

  int read_size_blocks = m_BufferLength;
  int read_size_bytes = read_size_blocks * m_SampleSize * m_ChannelCount;
  ALubyte* deinterleaved = new ALubyte[read_size_bytes];
  ALubyte* interleaved   = new ALubyte[read_size_bytes];
  ALuint* buffers = new ALuint[m_ChannelCount];

  // calculate the format of the data we'll stick in the AL buffers
  ALenum format = (m_BitsPerSample == 16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8);

  int buffer_length_bytes = m_BufferLength * m_SampleSize;
  while (processed_buffers--) {

    ReadDeinterleaved(deinterleaved, interleaved, read_size_blocks);

    // unqueue/refill/queue
    for (int i = 0; i < 1/*m_ChannelCount*/; i++) {
      alSourceUnqueueBuffers(m_Sources[i], 1, buffers + i);

      alBufferData(
        buffers[0],
        format,
        deinterleaved + buffer_length_bytes * i,
        buffer_length_bytes,
        m_SampleRate
      );

      alSourceQueueBuffers(m_Sources[i], 1, buffers + i);
    }
  }

  delete[] buffers;
  delete[] interleaved;
  delete[] deinterleaved;
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
void deinterleave(
  const T* interleaved,
  T* out,
  int channel_count,
  int sample_count)
{
  // deinterleave the samples
  int c = channel_count;
  while (c--) {

    const T* in = interleaved;

    int n = sample_count;
    while (n--) {
      *out = *in;
      in += channel_count;
      out++;
    }

    interleaved++;
  }
}

void
ALOutputStream::ReadDeinterleaved(
  void* deinterleaved,
  void* interleaved,
  int block_count)
{
  // samples are interleaved like: 01010101
  // return data like:             00001111
  
  int block_size = m_SampleSize * m_ChannelCount;

  // read samples
  int blocks_read = m_Source(m_Opaque, block_count, interleaved);
  if (blocks_read > 0) {
    // store the last sample read
    memcpy(m_LastBlock,
           (ALubyte*)interleaved + (blocks_read - 1) * block_size,
           block_size);
  }

  // fill the rest of the empty space
  int fill_left = block_count - blocks_read;
  ALubyte* fill = (ALubyte*)interleaved + blocks_read * block_size;
  while (fill_left--) {
    memcpy(fill, m_LastBlock, block_size);
    fill += block_size;
  }


  if (m_BitsPerSample == 16) {

    deinterleave<ALushort>(
      (ALushort*)interleaved,
      (ALushort*)deinterleaved,
      m_ChannelCount,
      block_count
    );

  } else {  // m_BitsPerSample == 8
    
    deinterleave<ALubyte>(
      (ALubyte*)interleaved,
      (ALubyte*)deinterleaved,
      m_ChannelCount,
      block_count
    );

  }
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::FillBuffers()
{
  int block_count = m_BufferLength * BUFFER_COUNT;
  int allocate = block_count * m_SampleSize * m_ChannelCount;
  ALubyte* deinterleaved = new ALubyte[allocate];
  ALubyte* interleaved   = new ALubyte[allocate];
  ReadDeinterleaved(deinterleaved, interleaved, block_count);
   
  // calculate the format of the data we'll stick in the AL buffers
  ALenum format = (m_BitsPerSample == 16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8);

  // stick the data into the buffers
  ALubyte* samples = deinterleaved;
  ALuint*  buffer  = m_Buffers;

  for (int i = 0; i < BUFFER_COUNT * m_ChannelCount; i++) {
      
    alBufferData(
      *buffer,
      format,
      samples,
      m_BufferLength * m_SampleSize,
      m_SampleRate
    );

    samples += m_BufferLength * m_SampleSize;
    buffer++;

  }

  delete[] deinterleaved;
  delete[] interleaved;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Play()
{
//  alSourcePlayv(m_ChannelCount, m_Sources);
  alSourcePlay(m_Sources[0]);
  m_IsPlaying = true;
}

////////////////////////////////////////////////////////////////////////////////

void
ALOutputStream::Stop()
{
//  alSourcePausev(m_ChannelCount, m_Sources);
  alSourcePause(m_Sources[0]);
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
  for (int i = 0; i < m_ChannelCount; i++) {
    alSourcef(m_Sources[i], AL_GAIN, v);
  }
}

////////////////////////////////////////////////////////////////////////////////

int
ALOutputStream::GetVolume()
{
  return m_Volume;
}

////////////////////////////////////////////////////////////////////////////////
