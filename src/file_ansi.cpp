#include <stdio.h>
#include "file.hpp"


////////////////////////////////////////////////////////////////////////////////

ADR_FILE ADR_CALL DefaultFileOpen(void* /*opaque*/, const char* filename)
{
  return (ADR_FILE)fopen(filename, "rb");
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL DefaultFileClose(ADR_FILE file)
{
  fclose((FILE*)file);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileRead(ADR_FILE file, void* buffer, int size)
{
  return fread(buffer, 1, size, (FILE*)file);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileSeek(
  ADR_FILE file,
  int destination,
  ADR_SEEK_TYPE origin)
{
  int o;
  switch (origin) {
    case ADR_BEGIN:   o = SEEK_SET; break;
    case ADR_CURRENT: o = SEEK_CUR; break;
    case ADR_END:     o = SEEK_END; break;
    default: return -1;
  }

  return (0 == fseek((FILE*)file, destination, o));
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileTell(ADR_FILE file)
{
  return ftell((FILE*)file);
}

////////////////////////////////////////////////////////////////////////////////
