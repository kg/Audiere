#include "stream.hpp"
#include "context.hpp"
#include "utility.hpp"


////////////////////////////////////////////////////////////////////////////////

Stream::Stream()
{
  m_context       = 0;
  m_input_stream  = 0;
  m_output_stream = 0;
}

////////////////////////////////////////////////////////////////////////////////

bool
Stream::Initialize(Context* context, const char* filename)
{
  ISampleSource* sample_source = OpenInputStream(
    context->m_file_system,
    filename);
  if (!sample_source) {
    return false;
  }

  m_input_stream = new RepeatableStream(sample_source, false);

  // open output stream
  m_output_stream = context->m_output_context->OpenStream(m_input_stream);
  if (!m_output_stream) {
    delete m_input_stream;
    m_input_stream = 0;
    return false;
  }

  m_context = context;
  m_context->AddRef();

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::Destructor()
{
  delete m_output_stream;
  m_output_stream = 0;

  delete m_input_stream;
  m_input_stream = 0;

  if (m_context) {
    m_context->Release();
    m_context = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

Stream::~Stream()
{
  if (m_context) {
    AI_Lock l__(m_context);
    Destructor();
  } else {
    Destructor();
  }
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::Play()
{
  AI_Lock l__(m_context);
  m_output_stream->Play();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::Pause()
{
  AI_Lock l__(m_context);
  m_output_stream->Stop();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::Reset()
{
  AI_Lock l__(m_context);
  m_input_stream->Reset();
  m_output_stream->Reset();
}

////////////////////////////////////////////////////////////////////////////////

bool
Stream::IsPlaying()
{
  AI_Lock l__(m_context);
  return m_output_stream->IsPlaying();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::SetRepeat(bool repeat)
{
  AI_Lock l__(m_context);
  m_input_stream->SetRepeat(repeat);
}

////////////////////////////////////////////////////////////////////////////////

bool
Stream::IsRepeating()
{
  AI_Lock l__(m_context);
  return m_input_stream->GetRepeat();
}

////////////////////////////////////////////////////////////////////////////////

void
Stream::SetVolume(int volume)
{
  AI_Lock l__(m_context);
  m_output_stream->SetVolume(volume);
}

////////////////////////////////////////////////////////////////////////////////

int
Stream::GetVolume()
{
  AI_Lock l__(m_context);
  return m_output_stream->GetVolume();
}

////////////////////////////////////////////////////////////////////////////////
