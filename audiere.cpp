#include <string.h>
#include <ctype.h>
#include <acoustique.h>
#include "nbase.hpp"
#include "audiere.h"
#include "default_io.hpp"
#include "output.hpp"

namespace nbase = naikai::base;


struct ADR_CONTEXT_STRUCT
{
  const char* output_device;

  // file I/O
  void*          opaque;
  ADR_FILE_OPEN  open;
  ADR_FILE_CLOSE close;
  ADR_FILE_READ  read;
  ADR_FILE_SEEK  seek;
  ADR_FILE_TELL  tell;

  int refcount;

  IOutputContext* output_context;

  // threading
  bool thread_should_die;
  nbase::CriticalSection cs;
};


struct ADR_STREAM_STRUCT
{
  ADR_CONTEXT_STRUCT* context;
  void*               file_handle;
  ACQ_STREAM          input_stream;
  IOutputStream*      output_stream;

  int sample_size;

  ADR_BOOL repeat;
};


static void ThreadRoutine(void* opaque);

static int  ACQ_CALL FileRead(void* opaque, void* bytes, int byte_count);
static void ACQ_CALL FileReset(void* opaque);

static int  ADR_CALL SampleSource(void* opaque, int sample_count, void* samples);
static void ADR_CALL SampleReset(void* opaque);


////////////////////////////////////////////////////////////////////////////////

const char* ADR_CALL AdrGetVersion()
{
  return "Audiere v1.01";
}

////////////////////////////////////////////////////////////////////////////////

ADR_CONTEXT ADR_CALL AdrCreateContext(
  const char* output_device,
  const char* parameters,
  void* opaque,
  ADR_FILE_OPEN open,
  ADR_FILE_CLOSE close,
  ADR_FILE_READ read,
  ADR_FILE_SEEK seek,
  ADR_FILE_TELL tell)
{
  // if any of the file callbacks are NULL, use the default ones
  if (!open || !close || !read || !seek || !tell) {
    open  = DefaultOpen;
    close = DefaultClose;
    read  = DefaultRead;
    seek  = DefaultSeek;
    tell  = DefaultTell;
  }

  // initialize the context
  ADR_CONTEXT_STRUCT* context = new ADR_CONTEXT_STRUCT;
  context->opaque = opaque;
  context->open   = open;
  context->close  = close;
  context->read   = read;
  context->seek   = seek;
  context->tell   = tell;
  
  context->refcount = 1;

  // create an output context
  context->output_context = OpenContext(output_device, parameters);
  if (!context->output_context) {
    delete context;
    return NULL;
  }

  // create a critical section
  context->cs = nbase::CreateCriticalSection();

  // create a thread
  context->thread_should_die = false;
  bool result = nbase::CreateThread(ThreadRoutine, context, +4);
  if (!result) {
    nbase::DestroyCriticalSection(context->cs);
    delete context->output_context;
    delete context;
    return NULL;
  }

  return context;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrDestroyContext(ADR_CONTEXT context)
{
  ADR_CONTEXT_STRUCT* internal = (ADR_CONTEXT_STRUCT*)context;

  // decrement the refcount
  internal->refcount--;
  if (internal->refcount == 0) {
  
    // ask the thread to die
    internal->thread_should_die = true;

    // wait until the thread is dead
    while (internal->thread_should_die) {
      nbase::Sleep(50);
    }

    nbase::DestroyCriticalSection(internal->cs);
  
    delete internal->output_context;
    delete internal;
  }
}

////////////////////////////////////////////////////////////////////////////////

void ThreadRoutine(void* opaque)
{
  ADR_CONTEXT_STRUCT* context = (ADR_CONTEXT_STRUCT*)opaque;

  // while we're not dead, process the sound update loop
  while (!context->thread_should_die) {
    
    nbase::EnterCriticalSection(context->cs);
    context->output_context->Update();
    nbase::LeaveCriticalSection(context->cs);
    nbase::Sleep(50);

  }

  // okay, we're dead now
  context->thread_should_die = false;
}

////////////////////////////////////////////////////////////////////////////////

bool case_strings_equal(const char* a, const char* b)
{
  while (*a && *b) {

    char c = tolower(*a++);
    char d = tolower(*b++);

    if (c != d) {
      return false;
    }
  }
  
  char c = tolower(*a);
  char d = tolower(*b);
  return (c == d);
}

ADR_STREAM ADR_CALL AdrOpenStream(ADR_CONTEXT context, const char* filename)
{
  ADR_STREAM_STRUCT* stream = new ADR_STREAM_STRUCT;
  stream->context    = (ADR_CONTEXT_STRUCT*)context;
  stream->repeat     = ADR_FALSE;

  // open file
  stream->file_handle = stream->context->open(
    stream->context->opaque,
    filename
  );
  if (!stream->file_handle) {
    delete stream;
    return NULL;
  }

  // extension -> stream type mapping table
  static const struct {
    const char* extension;
    int type;
  } extension_map[] = {
    { ".mp3", ACQ_STREAM_MP3 },
    { ".mp2", ACQ_STREAM_MP3 },
    { ".ogg", ACQ_STREAM_OGG },
    { ".wav", ACQ_STREAM_WAV },
    { ".mod", ACQ_STREAM_MOD },
    { ".s3m", ACQ_STREAM_MOD },
    { ".it",  ACQ_STREAM_MOD },
    { ".xm",  ACQ_STREAM_MOD },
  };

  // if filename has extension listed in table, use that format...
  // otherwise autodetect
  int stream_format = ACQ_STREAM_AUTODETECT;
  const int filename_length = strlen(filename);
  
  for (int i = 0; i < sizeof(extension_map) / sizeof(extension_map[0]); i++) {

    const char* extension = extension_map[i].extension;
    const int extension_length = strlen(extension);
    if (filename_length >= extension_length &&
        case_strings_equal(
          extension,
          filename + filename_length - extension_length
        )) {
      stream_format = extension_map[i].type;
      break;
    }

  }

  // open input stream
  stream->input_stream = AcqOpenStream(
    stream,
    FileRead,
    FileReset,
    stream_format
  );
  if (!stream->input_stream) {
    stream->context->close(stream->file_handle);
    delete stream;
    return NULL;
  }

  // get stream information
  int channel_count, bits_per_sample, sample_rate;
  AcqGetStreamInformation(
    stream->input_stream,
    &channel_count,
    &bits_per_sample,
    &sample_rate
  );

  // calculate sample size
  stream->sample_size = channel_count * bits_per_sample / 8;

  nbase::EnterCriticalSection(stream->context->cs);

  // open output stream
  stream->output_stream = stream->context->output_context->OpenStream(
    channel_count,
    sample_rate,
    bits_per_sample,
    SampleSource,
    SampleReset,
    stream
  );
  if (!stream->output_stream) {
    AcqCloseStream(stream->input_stream);
    stream->context->close(stream->context->opaque);
    delete stream;
    return NULL;
  }

  // increment the context's reference count
  stream->context->refcount++;
  
  nbase::LeaveCriticalSection(stream->context->cs);

  return stream;
}

////////////////////////////////////////////////////////////////////////////////

int ACQ_CALL FileRead(void* opaque, void* bytes, int byte_count)
{
  ADR_STREAM_STRUCT* stream = (ADR_STREAM_STRUCT*)opaque;
  return stream->context->read(stream->file_handle, bytes, byte_count);
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_CALL FileReset(void* opaque)
{
  ADR_STREAM_STRUCT* stream = (ADR_STREAM_STRUCT*)opaque;
  stream->context->seek(stream->file_handle, 0);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL SampleSource(void* opaque, const int sample_count, void* samples)
{
  ADR_STREAM_STRUCT* stream = (ADR_STREAM_STRUCT*)opaque;

  if (stream->repeat) {

    unsigned char* out = (unsigned char*)samples;
    int samples_left = sample_count;
    while (samples_left > 0) {

      // read some samples
      int samples_read = AcqReadStream(
        stream->input_stream,
        out,
        samples_left
      );

      // if we couldn't read anything, reset the stream and try again
      if (samples_read == 0) {
        AcqResetStream(stream->input_stream);
        samples_read = AcqReadStream(
          stream->input_stream,
          samples,
          samples_left
        );
      }

      // if we still can't read anything, we're done
      if (samples_read == 0) {
        break;
      }

      samples_left -= samples_read;
      out += samples_read * stream->sample_size;
    }

    return sample_count - samples_left;

  } else {

    return AcqReadStream(
      stream->input_stream,
      samples,
      sample_count
    );

  }
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL SampleReset(void* opaque)
{
  ADR_STREAM_STRUCT* stream = (ADR_STREAM_STRUCT*)opaque;
  AcqResetStream(stream->input_stream);
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrCloseStream(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;

  // close output_stream
  nbase::EnterCriticalSection(internal->context->cs);
  delete internal->output_stream;

  // close input stream
  AcqCloseStream(internal->input_stream);

  // close file
  internal->context->close(internal->file_handle);

  // we no longer need the context
  AdrDestroyContext(internal->context);

  nbase::LeaveCriticalSection(internal->context->cs);
  
  delete internal;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrPlayStream(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;

  // tell the output driver to start asking for samples
  internal->output_stream->Play();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrPauseStream(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;

  // tell the output driver to stop asking for samples
  internal->output_stream->Stop();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrResetStream(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;

  nbase::EnterCriticalSection(internal->context->cs);

  // reset the input stream
  AcqResetStream(internal->input_stream);

  // reset the output stream
  internal->output_stream->Reset();

  nbase::LeaveCriticalSection(internal->context->cs);
}

////////////////////////////////////////////////////////////////////////////////

ADR_BOOL ADR_CALL AdrIsStreamPlaying(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;
  return internal->output_stream->IsPlaying();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrSetStreamRepeat(ADR_STREAM stream, ADR_BOOL repeat)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;
  internal->repeat = repeat;
}

////////////////////////////////////////////////////////////////////////////////

ADR_BOOL ADR_CALL AdrGetStreamRepeat(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;
  return internal->repeat;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrSetStreamVolume(ADR_STREAM stream, int volume)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;
  internal->output_stream->SetVolume(volume);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL AdrGetStreamVolume(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;
  return internal->output_stream->GetVolume();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrSetStreamPan(ADR_STREAM stream, int pan)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;
  internal->output_stream->SetPan(pan);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL AdrGetStreamPan(ADR_STREAM stream)
{
  ADR_STREAM_STRUCT* internal = (ADR_STREAM_STRUCT*)stream;
  return internal->output_stream->GetPan();
}

////////////////////////////////////////////////////////////////////////////////
