#include <string>
#include <memory>
#include <string.h>
#include <ctype.h>
#include <acoustique.h>
#include "audiere.h"
#include "file.hpp"
#include "output.hpp"
#include "threads.hpp"


struct ADR_CONTEXT_ATTRimp
{
  std::string    output_device;
  std::string    parameters;
  void*          opaque;
  ADR_FILE_OPEN  open;
  ADR_FILE_CLOSE close;
  ADR_FILE_READ  read;
  ADR_FILE_SEEK  seek;
  ADR_FILE_TELL  tell;
};


struct ADR_CONTEXTimp
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
  AI_CriticalSection cs;
};


struct ADR_STREAMimp : public ISampleSource
{
  // ISampleSource implementation
  // yeah, this is kinda hacky
  void GetFormat(
    int& channel_count,
    int& sample_rate,
    int& bits_per_sample)
  {
    AcqGetStreamInformation(
      input_stream,
      &channel_count,
      &bits_per_sample,
      &sample_rate);
  }

  int Read(int sample_count, void* samples)
  {
    if (repeat) {

      unsigned char* out = (unsigned char*)samples;
      int samples_left = sample_count;
      while (samples_left > 0) {

        // read some samples
        int samples_read = AcqReadStream(input_stream, out, samples_left);

        // if we couldn't read anything, reset the stream and try again
        if (samples_read == 0) {
          AcqResetStream(input_stream);
          samples_read = AcqReadStream(input_stream, samples, samples_left);
        }

        // if we still can't read anything, we're done
        if (samples_read == 0) {
          break;
        }

        samples_left -= samples_read;
        out += samples_read * sample_size;
      }

      return sample_count - samples_left;

    } else {

      return AcqReadStream(input_stream, samples, sample_count);

    }
  }

  void Reset()
  {
    AcqResetStream(input_stream);
  }

  // the struct fields
  ADR_CONTEXT    context;
  ADR_FILE       file_handle;
  ACQ_STREAM     input_stream;
  IOutputStream* output_stream;

  int sample_size;

  ADR_BOOL repeat;
};


static void ThreadRoutine(void* opaque);

static int  ACQ_CALL FileRead(void* opaque, void* bytes, int byte_count);
static void ACQ_CALL FileReset(void* opaque);


////////////////////////////////////////////////////////////////////////////////

const char* ADR_CALL AdrGetVersion()
{
  return "1.0.3";
}

////////////////////////////////////////////////////////////////////////////////

ADR_CONTEXT_ATTR ADR_CALL AdrCreateContextAttr()
{
  ADR_CONTEXT_ATTR attr = new ADR_CONTEXT_ATTRimp;
  attr->output_device = "";
  attr->parameters    = "";
  attr->opaque        = 0;
  attr->open          = DefaultFileOpen;
  attr->close         = DefaultFileClose;
  attr->read          = DefaultFileRead;
  attr->seek          = DefaultFileSeek;
  attr->tell          = DefaultFileTell;
  return attr;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrDestroyContextAttr(
  ADR_CONTEXT_ATTR attr)
{
  delete attr;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrContextAttrSetOutputDevice(
  ADR_CONTEXT_ATTR attr,
  const char* output_device)
{
  attr->output_device = output_device;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrContextAttrSetParameters(
  ADR_CONTEXT_ATTR attr,
  const char* parameters)
{
  attr->parameters = parameters;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrContextAttrSetOpaque(
  ADR_CONTEXT_ATTR attr,
  void* opaque)
{
  attr->opaque = opaque;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrContextAttrSetFileCallbacks(
  ADR_CONTEXT_ATTR attr,
  ADR_FILE_OPEN  open,
  ADR_FILE_CLOSE close,
  ADR_FILE_READ  read,
  ADR_FILE_SEEK  seek,
  ADR_FILE_TELL  tell)
{
  if (!open || !close || !read || !seek || !tell) {
    attr->open  = DefaultFileOpen;
    attr->close = DefaultFileClose;
    attr->read  = DefaultFileRead;
    attr->seek  = DefaultFileSeek;
    attr->tell  = DefaultFileTell;
  } else {
    attr->open  = open;
    attr->close = close;
    attr->read  = read;
    attr->seek  = seek;
    attr->tell  = tell;
  }
}

////////////////////////////////////////////////////////////////////////////////

ADR_CONTEXT ADR_CALL AdrCreateContext(
  ADR_CONTEXT_ATTR attr)
{
  ADR_CONTEXT_ATTR default_attr = 0;
  if (!attr) {
    attr = default_attr = AdrCreateContextAttr();
  }

  // initialize the context
  ADR_CONTEXT context = new ADR_CONTEXTimp;
  context->opaque = attr->opaque;
  context->open   = attr->open;
  context->close  = attr->close;
  context->read   = attr->read;
  context->seek   = attr->seek;
  context->tell   = attr->tell;

  context->refcount = 1;

  // create an output context
  context->output_context = OpenContext(
    attr->output_device.c_str(),
    attr->parameters.c_str());
  if (!context->output_context) {
    delete context;
    delete default_attr;
    return NULL;
  }

  // create a critical section
  context->cs = AI_CreateCriticalSection();

  // create a thread
  context->thread_should_die = false;
  bool result = AI_CreateThread(ThreadRoutine, context, +4);
  if (!result) {
    AI_DestroyCriticalSection(context->cs);
    delete context->output_context;
    delete context;
    delete default_attr;
    return NULL;
  }

  delete default_attr;
  return context;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrDestroyContext(ADR_CONTEXT context)
{
  context->refcount--;
  if (context->refcount == 0) {
  
    // ask the thread to die
    context->thread_should_die = true;

    // wait until the thread is dead
    while (context->thread_should_die) {
      AI_Sleep(50);
    }

    AI_DestroyCriticalSection(context->cs);
  
    delete context->output_context;
    delete context;
  }
}

////////////////////////////////////////////////////////////////////////////////

void ThreadRoutine(void* opaque)
{
  ADR_CONTEXT context = (ADR_CONTEXT)opaque;

  // while we're not dead, process the sound update loop
  while (!context->thread_should_die) {
    
    AI_Lock l(context->cs);
    context->output_context->Update();

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
  ADR_STREAM stream = new ADR_STREAMimp;
  stream->context = context;
  stream->repeat  = ADR_FALSE;

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

  AI_Lock l(stream->context->cs);

  // open output stream
  stream->output_stream = stream->context->output_context->OpenStream(stream);
  if (!stream->output_stream) {
    AcqCloseStream(stream->input_stream);
    stream->context->close(stream->file_handle);
    delete stream;
    return NULL;
  }

  // increment the context's reference count
  stream->context->refcount++;
  
  return stream;
}

////////////////////////////////////////////////////////////////////////////////

int ACQ_CALL FileRead(void* opaque, void* bytes, int byte_count)
{
  ADR_STREAM stream = (ADR_STREAM)opaque;
  return stream->context->read(stream->file_handle, bytes, byte_count);
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_CALL FileReset(void* opaque)
{
  ADR_STREAM stream = (ADR_STREAM)opaque;
  stream->context->seek(stream->file_handle, 0);
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrCloseStream(ADR_STREAM stream)
{
  // close output_stream
  AI_Lock l(stream->context->cs);
  delete stream->output_stream;

  // close input stream
  AcqCloseStream(stream->input_stream);

  // close file
  stream->context->close(stream->file_handle);

  // we no longer need the context
  AdrDestroyContext(stream->context);

  delete stream;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrPlayStream(ADR_STREAM stream)
{
  AI_Lock l(stream->context->cs);

  // tell the output driver to start asking for samples
  stream->output_stream->Play();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrPauseStream(ADR_STREAM stream)
{
  AI_Lock l(stream->context->cs);

  // tell the output driver to stop asking for samples
  stream->output_stream->Stop();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrResetStream(ADR_STREAM stream)
{
  AI_Lock l(stream->context->cs);

  // reset the input stream
  AcqResetStream(stream->input_stream);

  // reset the output stream
  stream->output_stream->Reset();
}

////////////////////////////////////////////////////////////////////////////////

ADR_BOOL ADR_CALL AdrIsStreamPlaying(ADR_STREAM stream)
{
  AI_Lock l(stream->context->cs);
  return stream->output_stream->IsPlaying();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrSetStreamRepeat(ADR_STREAM stream, ADR_BOOL repeat)
{
  AI_Lock l(stream->context->cs);
  stream->repeat = repeat;
}

////////////////////////////////////////////////////////////////////////////////

ADR_BOOL ADR_CALL AdrGetStreamRepeat(ADR_STREAM stream)
{
  AI_Lock l(stream->context->cs);
  return stream->repeat;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrSetStreamVolume(ADR_STREAM stream, int volume)
{
  // make sure the volume is within a valid range
  if (volume < ADR_VOLUME_MIN) {
    volume = ADR_VOLUME_MIN;
  } else if (volume > ADR_VOLUME_MAX) {
    volume = ADR_VOLUME_MAX;
  }

  AI_Lock l(stream->context->cs);
  stream->output_stream->SetVolume(volume);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL AdrGetStreamVolume(ADR_STREAM stream)
{
  AI_Lock l(stream->context->cs);
  return stream->output_stream->GetVolume();
}

////////////////////////////////////////////////////////////////////////////////
