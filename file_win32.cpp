#include <windows.h>
#include "file.hpp"


// WIN32_FILE
struct ADR_FileHandle
{
  HANDLE handle;
};


static ADR_FILE DefaultFileOpenA(const char* filename);


////////////////////////////////////////////////////////////////////////////////

ADR_FILE ADR_CALL DefaultFileOpen(void* /*opaque*/, const char* filename)
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

  ADR_FileHandle* file = new ADR_FileHandle;
  file->handle = handle;
  return file;
}

////////////////////////////////////////////////////////////////////////////////

ADR_FILE DefaultFileOpenA(const char* filename)
{
  HANDLE handle = CreateFileA(
    filename, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (handle == INVALID_HANDLE_VALUE) {
    return NULL;
  }

  ADR_FileHandle* file = new ADR_FileHandle;
  file->handle = handle;
  return file;
}

////////////////////////////////////////////////////////////////////////////////

void ADR_CALL DefaultFileClose(ADR_FILE file)
{
  CloseHandle(file->handle);
  delete file;
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileRead(ADR_FILE file, void* buffer, int size)
{
  DWORD read;
  BOOL result = ReadFile(file->handle, buffer, size, &read, NULL);
  return (result ? read : 0);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileSeek(ADR_FILE file, int destination)
{
  int d = SetFilePointer(file->handle, destination, NULL, FILE_BEGIN);
  return (d != destination);
}

////////////////////////////////////////////////////////////////////////////////

int ADR_CALL DefaultFileTell(ADR_FILE file)
{
  return SetFilePointer(file->handle, 0, NULL, FILE_CURRENT);
}

////////////////////////////////////////////////////////////////////////////////
