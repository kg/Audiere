#include "context.hpp"
#include "stream.hpp"
#include "threads.hpp"


////////////////////////////////////////////////////////////////////////////////

Context* Context::Create(
  const char* output_device,
  const char* parameters,
  IFileSystem* file_system)
{
  Context* context = new Context();
  if (context->Initialize(output_device, parameters, file_system)) {
    return context;
  } else {
    delete context;
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

Context::Context()
{
  m_ref_count = 0;
  m_thread_should_die = false;
  m_file_system = 0;
  m_output_context = 0;
}

////////////////////////////////////////////////////////////////////////////////

Context::~Context()
{
  // ask the thread to die
  m_thread_should_die = true;

  // wait until the thread is dead
  while (m_thread_should_die) {
    AI_Sleep(50);
  }

  delete m_output_context;
  delete m_file_system;
}

////////////////////////////////////////////////////////////////////////////////

bool
Context::Initialize(
  const char* output_device,
  const char* parameters,
  IFileSystem* file_system)
{
  m_ref_count = 1;
  m_thread_should_die = false;

  m_file_system = file_system;

  m_output_context = OpenContext(output_device, parameters);
  if (!m_output_context) {
    return false;
  }

  // create a thread
  // this is particularly odd:  don't create a thread with higher priority than
  // 0 on Win9x, or some race condition occurs and the update thread completely
  // starves the main thread of the program.
  //
  // the race condition is related to the locks (critical sections)...
  bool result = AI_CreateThread(ThreadRoutine, this, 0);  /* +4  */
  if (!result) {
    delete m_output_context;
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
Context::AddRef()
{
  ++m_ref_count;
}

////////////////////////////////////////////////////////////////////////////////

void
Context::Release()
{
  if (--m_ref_count == 0) {
    delete this;
  }
}

////////////////////////////////////////////////////////////////////////////////

void
Context::ThreadRoutine(void* opaque)
{
  Context* context = reinterpret_cast<Context*>(opaque);
  context->ThreadRoutine();
}

////////////////////////////////////////////////////////////////////////////////

void
Context::ThreadRoutine()
{
  // while we're not dead, process the sound update loop
  while (!m_thread_should_die) {
    ADR_LOG("Updating output context...");
    ::AI_Lock l__(this);
    m_output_context->Update();
  }

  // okay, we're dead now
  m_thread_should_die = false;
}

////////////////////////////////////////////////////////////////////////////////

Stream*
Context::OpenStream(const char* filename)
{
  ADR_GUARD("Context::OpenStream");

  AI_Lock l__(this);

  Stream* stream = new Stream();
  if (stream->Initialize(this, filename)) {
    return stream;
  } else {
    delete stream;
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
