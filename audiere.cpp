#include <string>
#include <string.h>
#include <ctype.h>
#include <acoustique.h>
#include "audiere.h"
#include "context.hpp"
#include "stream.hpp"
#include "file.hpp"


inline Context* CONTEXT(ADR_CONTEXT c) { return reinterpret_cast<Context*>(c); }
inline Stream*  STREAM(ADR_STREAM s)   { return reinterpret_cast<Stream*>(s);  }


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
  ADR_GUARD("AdrCreateContext");

  ADR_CONTEXT_ATTR delete_attr = 0;
  if (!attr) {
    delete_attr = AdrCreateContextAttr();
    attr = delete_attr;
  }

  Context* context = Context::Create(attr);
  delete delete_attr;
  return reinterpret_cast<ADR_CONTEXT>(context);
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrDestroyContext(ADR_CONTEXT context)
{
  ADR_GUARD("AdrDestroyContext");

  CONTEXT(context)->Release();
}

////////////////////////////////////////////////////////////////////////////////

ADR_STREAM ADR_CALL AdrOpenStream(ADR_CONTEXT context, const char* filename)
{
  ADR_GUARD("AdrOpenStream");

  return reinterpret_cast<ADR_STREAM>(CONTEXT(context)->OpenStream(filename));
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrCloseStream(ADR_STREAM stream)
{
  ADR_GUARD("AdrCloseStream");

  delete STREAM(stream);
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrPlayStream(ADR_STREAM stream)
{
  ADR_GUARD("AdrPlayStream");

  STREAM(stream)->Play();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrPauseStream(ADR_STREAM stream)
{
  ADR_GUARD("AdrPauseStream");

  STREAM(stream)->Pause();
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrResetStream(ADR_STREAM stream)
{
  ADR_GUARD("AdrResetStream");

  STREAM(stream)->ResetInputAndOutput();
}

////////////////////////////////////////////////////////////////////////////////

ADR_BOOL ADR_CALL AdrIsStreamPlaying(ADR_STREAM stream)
{
  ADR_GUARD("AdrIsStreamPlaying");

  return STREAM(stream)->IsPlaying() ? ADR_TRUE : ADR_FALSE;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrSetStreamRepeat(ADR_STREAM stream, ADR_BOOL repeat)
{
  ADR_GUARD("AdrSetStreamRepeat");

  STREAM(stream)->SetRepeat(repeat == ADR_TRUE);
}

////////////////////////////////////////////////////////////////////////////////

ADR_BOOL ADR_CALL AdrGetStreamRepeat(ADR_STREAM stream)
{
  ADR_GUARD("AdrGetStreamRepeat");

  return STREAM(stream)->IsRepeating() ? ADR_TRUE : ADR_FALSE;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL AdrSetStreamVolume(ADR_STREAM stream, int volume)
{
  ADR_GUARD("AdrSetStreamVolume");

  // make sure the volume is within a valid range
  if (volume < ADR_VOLUME_MIN) {
    volume = ADR_VOLUME_MIN;
  } else if (volume > ADR_VOLUME_MAX) {
    volume = ADR_VOLUME_MAX;
  }

  STREAM(stream)->SetVolume(volume);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL AdrGetStreamVolume(ADR_STREAM stream)
{
  ADR_GUARD("AdrGetStreamVolume");

  return STREAM(stream)->GetVolume();
}

////////////////////////////////////////////////////////////////////////////////
