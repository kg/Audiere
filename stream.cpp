#include "stream.hpp"
#include "context.hpp"


////////////////////////////////////////////////////////////////////////////////

bool
Stream::Initialize(Context* context, const char* filename)
{
  m_context = context;

  // open file
  m_file = m_context->m_open(m_context->m_opaque, filename);
  if (!m_file) {
    return false;
  }

  // open input stream
  m_input_stream = AcqOpenStream(
    this,
    FileRead,
    FileReset,
    GetFileFormat(filename));
  if (!m_input_stream) {
    m_context->m_close(m_file);
    return false;
  }

  // open output stream
  m_output_stream = m_context->m_output_context->OpenStream(this);
  if (!m_output_stream) {
    AcqCloseStream(m_input_stream);
    m_context->m_close(m_file);
    return false;
  }

  // get stream information
  int channel_count, bits_per_sample, sample_rate;
  AcqGetStreamInformation(
    m_input_stream,
    &channel_count,
    &bits_per_sample,
    &sample_rate);

  // calculate sample size
  m_sample_size = channel_count * bits_per_sample / 8;

  m_context->AddRef();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

Stream::~Stream()
{
  delete m_output_stream;
  AcqCloseStream(m_input_stream);
  m_context->m_close(m_file);
  m_context->Release();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::Play()
{
  AI_Lock l__(this);
  m_output_stream->Play();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::Pause()
{
  AI_Lock l__(this);
  m_output_stream->Stop();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::ResetInputAndOutput()
{
  AI_Lock l__(this);
  AcqResetStream(m_input_stream);
  m_output_stream->Reset();
}

////////////////////////////////////////////////////////////////////////////////

bool
Stream::IsPlaying()
{
  AI_Lock l__(this);
  return m_output_stream->IsPlaying();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::SetRepeat(bool repeat)
{
  AI_Lock l__(this);
  m_repeat = repeat;
}

////////////////////////////////////////////////////////////////////////////////

bool
Stream::IsRepeating()
{
  AI_Lock l__(this);
  return m_repeat;
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::SetVolume(int volume)
{
  AI_Lock l__(this);
  m_output_stream->SetVolume(volume);
}

////////////////////////////////////////////////////////////////////////////////

int
Stream::GetVolume()
{
  AI_Lock l__(this);
  return m_output_stream->GetVolume();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::GetFormat(int& channel_count, int& sample_rate, int& bits_per_sample)
{
  AcqGetStreamInformation(
    m_input_stream,
    &channel_count,
    &bits_per_sample,
    &sample_rate);
}

////////////////////////////////////////////////////////////////////////////////

int
Stream::Read(int sample_count, void* samples)
{
  if (m_repeat) {

    unsigned char* out = (unsigned char*)samples;
    int samples_left = sample_count;
    while (samples_left > 0) {

      // read some samples
      int samples_read = AcqReadStream(m_input_stream, out, samples_left);

      // if we couldn't read anything, reset the stream and try again
      if (samples_read == 0) {
        AcqResetStream(m_input_stream);
        samples_read = AcqReadStream(m_input_stream, samples, samples_left);
      }

      // if we still can't read anything, we're done
      if (samples_read == 0) {
        break;
      }

      samples_left -= samples_read;
      out += samples_read * m_sample_size;
    }

    return sample_count - samples_left;

  } else {

    return AcqReadStream(m_input_stream, samples, sample_count);

  }
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::Reset()
{
  AcqResetStream(m_input_stream);
}

////////////////////////////////////////////////////////////////////////////////

int ACQ_CALL
Stream::FileRead(void* opaque, void* bytes, int byte_count)
{
  Stream* stream = (Stream*)opaque;
  return stream->m_context->m_read(stream->m_file, bytes, byte_count);
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_CALL
Stream::FileReset(void* opaque)
{
  Stream* stream = (Stream*)opaque;
  stream->m_context->m_seek(stream->m_file, 0);
}

////////////////////////////////////////////////////////////////////////////////

static int strcmp_case(const char* a, const char* b)
{
  while (*a && *b) {

    char c = tolower(*a++);
    char d = tolower(*b++);

    if (c != d) {
      return c - d;
    }
  }
  
  char c = tolower(*a);
  char d = tolower(*b);
  return (c - d);
}

////////////////////////////////////////////////////////////////////////////////

int
Stream::GetFileFormat(const char* filename)
{
  // extension -> stream type mapping table
  static const struct {
    const char* extension;
    int type;
  } extension_map[] = {
    { ".ogg", ACQ_STREAM_OGG },
    { ".wav", ACQ_STREAM_WAV },
    { ".mod", ACQ_STREAM_MOD },
    { ".s3m", ACQ_STREAM_MOD },
    { ".it",  ACQ_STREAM_MOD },
    { ".xm",  ACQ_STREAM_MOD },
  };

  static const int extension_map_size =
    sizeof(extension_map) / sizeof(extension_map[0]);


  const int filename_length = strlen(filename);

  // if filename has extension listed in table, use that format...
  for (int i = 0; i < extension_map_size; i++) {

    const char* extension = extension_map[i].extension;
    const int extension_length = strlen(extension);
    const char* end = filename + filename_length - extension_length;
    
    if (filename_length >= extension_length &&
        strcmp_case(extension, end) == 0) {
      return extension_map[i].type;
    }
  }

  // otherwise autodetect
  return ACQ_STREAM_AUTODETECT;
}

////////////////////////////////////////////////////////////////////////////////
