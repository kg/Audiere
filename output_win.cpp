#include <windows.h>
#include "output.hpp"


#define SA_CALL __stdcall

typedef void* SA_STREAM;
typedef int (SA_CALL *SA_SAMPLE_SOURCE)(
  void* opaque,
  int sample_count,
  void* samples);


struct ADR_OUTPUT_CONTEXT_INTERNAL
{
  // anonymous window
  HWND window;

  // Sphere audio driver
  HINSTANCE audio_dll;

  // Sphere audio functions
  bool (SA_CALL *SA_OpenDriver)(HWND window);
  void (SA_CALL *SA_CloseDriver)();

  void (SA_CALL *SA_Update)();

  SA_STREAM (SA_CALL *SA_OpenStream)(
    int channel_count,
    int sample_rate,
    int bits_per_sample,
    SA_SAMPLE_SOURCE source,
    void* opaque);

  void (SA_CALL *SA_CloseStream)(SA_STREAM stream);

  void (SA_CALL *SA_PlayStream)(SA_STREAM stream);
  void (SA_CALL *SA_StopStream)(SA_STREAM stream);

  void (SA_CALL *SA_SetVolume)(SA_STREAM stream, int volume);
  int  (SA_CALL *SA_GetVolume)(SA_STREAM stream);
  void (SA_CALL *SA_SetPan)(SA_STREAM stream, int pan);
  int  (SA_CALL *SA_GetPan)(SA_STREAM stream);
};


struct SOURCE_INTERNAL
{
  void*             opaque;
  ADR_SAMPLE_SOURCE source;
};


struct ADR_OUTPUT_STREAM_INTERNAL
{
  ADR_OUTPUT_CONTEXT_INTERNAL* context;
  SA_STREAM output_stream;

  // needed for an extra layer of indirection
  SOURCE_INTERNAL source_internal;
};


////////////////////////////////////////////////////////////////////////////////

template<typename T>
void assign(T& dest, FARPROC src) {
  dest = reinterpret_cast<T>(src);
}

static LRESULT CALLBACK WindowProc(
  HWND window,
  UINT message,
  WPARAM wparam,
  LPARAM lparam)
{
  return DefWindowProc(window, message, wparam, lparam);
}

ADR_OUTPUT_CONTEXT out_OpenContext(const char* output_device)
{
  // calculate the name of the audio DLL
  static const char* prefix = "driver=";
  static const int prefix_length = 7;
  if (strlen(output_device) < prefix_length) {
    return NULL;
  }
  const char* audio_driver = output_device + prefix_length;

  ADR_OUTPUT_CONTEXT_INTERNAL* context = new ADR_OUTPUT_CONTEXT_INTERNAL;
  
  // register window class
  // don't worry about failure, if it fails, the window creation will fail
  WNDCLASS wc;
  wc.style         = 0;
  wc.lpfnWndProc   = WindowProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = GetModuleHandle(NULL);
  wc.hIcon         = NULL;
  wc.hCursor       = NULL;
  wc.hbrBackground = NULL;
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "AudiereHiddenWindow";
  RegisterClass(&wc);

  // create an anonymous window
  context->window = CreateWindow(
    "AudiereHiddenWindow", "", WS_POPUP,
    0, 0, 0, 0,
    NULL, NULL, GetModuleHandle(NULL), NULL);
  if (!context->window) {
    delete context;
    return NULL;
  }

  // open the Sphere audio DLL
  context->audio_dll = LoadLibrary(audio_driver);
  if (!context->audio_dll) {
    delete context;
    return NULL;
  }

  #define ASSIGN(d, s) assign(d, GetProcAddress(context->audio_dll, s))

  // get the address of all of the functions we need
  ASSIGN(context->SA_OpenDriver,  "SA_OpenDriver");
  ASSIGN(context->SA_CloseDriver, "SA_CloseDriver");
  ASSIGN(context->SA_Update,      "SA_Update");
  ASSIGN(context->SA_OpenStream,  "SA_OpenStream");
  ASSIGN(context->SA_CloseStream, "SA_CloseStream");
  ASSIGN(context->SA_PlayStream,  "SA_PlayStream");
  ASSIGN(context->SA_StopStream,  "SA_StopStream");
  ASSIGN(context->SA_SetVolume,   "SA_SetVolume");
  ASSIGN(context->SA_GetVolume,   "SA_GetVolume");
  ASSIGN(context->SA_SetPan,      "SA_SetPan");
  ASSIGN(context->SA_GetPan,      "SA_GetPan");

  // if any of the functions are NULL, we failed
  if (!context->SA_OpenDriver  ||
      !context->SA_CloseDriver ||
      !context->SA_Update      ||
      !context->SA_OpenStream  ||
      !context->SA_CloseStream ||
      !context->SA_PlayStream  ||
      !context->SA_StopStream  ||
      !context->SA_SetVolume   ||
      !context->SA_GetVolume   ||
      !context->SA_SetPan      ||
      !context->SA_GetPan) {
    
    FreeLibrary(context->audio_dll);
    delete context;
    return NULL;
  }

  // now initialize the driver
  if (!context->SA_OpenDriver(context->window)) {
    FreeLibrary(context->audio_dll);
    delete context;
    return NULL;
  }

  return context;
}

////////////////////////////////////////////////////////////////////////////////

void out_CloseContext(ADR_OUTPUT_CONTEXT context)
{
  ADR_OUTPUT_CONTEXT_INTERNAL* internal = (ADR_OUTPUT_CONTEXT_INTERNAL*)context;

  // close the driver
  internal->SA_CloseDriver();

  // free the audio DLL
  FreeLibrary(internal->audio_dll);
  delete context;
}

////////////////////////////////////////////////////////////////////////////////

void out_Update(ADR_OUTPUT_CONTEXT context)
{
  ADR_OUTPUT_CONTEXT_INTERNAL* internal = (ADR_OUTPUT_CONTEXT_INTERNAL*)context;
  internal->SA_Update();
}

////////////////////////////////////////////////////////////////////////////////

static int SA_CALL SampleSource(void* opaque, int sample_count, void* samples)
{
  SOURCE_INTERNAL* internal = (SOURCE_INTERNAL*)opaque;
  return internal->source(internal->opaque, sample_count, samples);
}

ADR_OUTPUT_STREAM out_OpenStream(
  ADR_OUTPUT_CONTEXT context,
  int channel_count,
  int sample_rate,
  int bits_per_sample,
  ADR_SAMPLE_SOURCE source,
  void* opaque)
{
  ADR_OUTPUT_STREAM_INTERNAL* stream = new ADR_OUTPUT_STREAM_INTERNAL;
  stream->context = (ADR_OUTPUT_CONTEXT_INTERNAL*)context;

  stream->source_internal.opaque = opaque;
  stream->source_internal.source = source;  
  
  // open output stream
  stream->output_stream = stream->context->SA_OpenStream(
    channel_count,
    sample_rate,
    bits_per_sample,
    SampleSource,
    &stream->source_internal
  );

  if (!stream->output_stream) {
    delete stream;
    return NULL;
  }

  return stream;
}

////////////////////////////////////////////////////////////////////////////////

void out_CloseStream(ADR_OUTPUT_STREAM stream)
{
  ADR_OUTPUT_STREAM_INTERNAL* internal = (ADR_OUTPUT_STREAM_INTERNAL*)stream;
  
  // close the output driver
  internal->context->SA_CloseStream(internal->output_stream);

  delete internal;
}

////////////////////////////////////////////////////////////////////////////////

void out_PlayStream(ADR_OUTPUT_STREAM stream)
{
  ADR_OUTPUT_STREAM_INTERNAL* internal = (ADR_OUTPUT_STREAM_INTERNAL*)stream;
  internal->context->SA_PlayStream(internal->output_stream);
}

////////////////////////////////////////////////////////////////////////////////

void out_StopStream(ADR_OUTPUT_STREAM stream)
{
  ADR_OUTPUT_STREAM_INTERNAL* internal = (ADR_OUTPUT_STREAM_INTERNAL*)stream;
  internal->context->SA_StopStream(internal->output_stream);
}

////////////////////////////////////////////////////////////////////////////////

void out_SetVolume(ADR_OUTPUT_STREAM stream, int volume)
{
  ADR_OUTPUT_STREAM_INTERNAL* internal = (ADR_OUTPUT_STREAM_INTERNAL*)stream;
  internal->context->SA_SetVolume(internal->output_stream, volume);
}

////////////////////////////////////////////////////////////////////////////////

int out_GetVolume(ADR_OUTPUT_STREAM stream)
{
  ADR_OUTPUT_STREAM_INTERNAL* internal = (ADR_OUTPUT_STREAM_INTERNAL*)stream;
  return internal->context->SA_GetVolume(internal->output_stream);
}

////////////////////////////////////////////////////////////////////////////////

void out_SetPan(ADR_OUTPUT_STREAM stream, int pan)
{
  ADR_OUTPUT_STREAM_INTERNAL* internal = (ADR_OUTPUT_STREAM_INTERNAL*)stream;
  internal->context->SA_SetPan(internal->output_stream, pan);
}

////////////////////////////////////////////////////////////////////////////////

int out_GetPan(ADR_OUTPUT_STREAM stream)
{
  ADR_OUTPUT_STREAM_INTERNAL* internal = (ADR_OUTPUT_STREAM_INTERNAL*)stream;
  return internal->context->SA_GetPan(internal->output_stream);
}

////////////////////////////////////////////////////////////////////////////////
