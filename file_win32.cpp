#include <windows.h>
#include "file.hpp"


struct WIN32_FILE
{
  HANDLE handle;
};


static void* DefaultFileOpenA(const char* filename);


////////////////////////////////////////////////////////////////////////////////

void* ADR_CALL DefaultFileOpen(void* /*opaque*/, const char* filename)
{
  // first, let's try to convert the UTF-8 filename to a wide string
  // calculate length of UTF-8 string
  int wfilename_length = MultiByteToWideChar(
    CP_UTF8, 0,
    filename, -1,
    0, NULL);
  
  // do the conversion now
  WCHAR* wfilename = new WCHAR[wfilename_length + 1];
  wfilename[wfilename_length] = 0;
  int result = MultiByteToWideChar(
    CP_UTF8, 0,
    filename, -1,
    wfilename, wfilename_length + 1);

  if (result == 0) {
    delete[] wfilename;
    return DefaultFileOpenA(filename);
  }

  HANDLE handle = CreateFileW(
    wfilename, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (handle == INVALID_HANDLE_VALUE) {
    return DefaultFileOpenA(filename);
  }

  delete[] wfilename;

  WIN32_FILE* file = new WIN32_FILE;
  file->handle = handle;
  return file;
}

////////////////////////////////////////////////////////////////////////////////

void* DefaultFileOpenA(const char* filename)
{
  HANDLE handle = CreateFileA(
    filename, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (handle == INVALID_HANDLE_VALUE) {
    return NULL;
  }

  WIN32_FILE* f = new WIN32_FILE;
  f->handle = handle;
  return f;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL DefaultFileClose(void* file)
{
  WIN32_FILE* f = (WIN32_FILE*)file;
  CloseHandle(f->handle);
  delete f;
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileRead(void* file, void* buffer, int size)
{
  WIN32_FILE* f = (WIN32_FILE*)file;
  DWORD read;
  BOOL result = ReadFile(f->handle, buffer, size, &read, NULL);
  if (!result) {
    return 0;
  } else {
    return read;
  }
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileSeek(void* file, int destination)
{
  WIN32_FILE* f = (WIN32_FILE*)file;
  int d = SetFilePointer(f->handle, destination, NULL, FILE_BEGIN);
  return (d == destination);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileTell(void* file)
{
  WIN32_FILE* f = (WIN32_FILE*)file;
  return SetFilePointer(f->handle, 0, NULL, FILE_CURRENT);
}

////////////////////////////////////////////////////////////////////////////////
