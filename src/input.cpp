#include <string.h>
#include "input.hpp"
#include "debug.hpp"
#include "input_mod.hpp"
#include "input_ogg.hpp"
#include "input_wav.hpp"
#include "utility.hpp"


////////////////////////////////////////////////////////////////////////////////

inline bool end_is(const char* begin, const char* end, const char* ext) {
  int ext_length = strlen(ext);
  if (ext_length > end - begin) {
    return false;
  } else {
    return (strcmp_case(end - ext_length, ext) == 0);
  }
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
static T* TryInputStream(IFile* file) {

  // initialize should never close the file

  T* source = new T();
  if (source->Initialize(file)) {
    return source;
  } else {
    delete source;
    return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

ISampleSource* OpenInputStream(IFileSystem* fs, const char* filename)
{
  ADR_GUARD("OpenInputStream");

  // try to open the file
  IFile* file = fs->Open(filename);
  if (!file) {
    return 0;
  }
  
  ISampleSource* source;

  #define TRY_SOURCE(source_type)                \
    source = TryInputStream<source_type>(file);  \
    if (source) {                                \
      return source;                             \
    } else {                                     \
      file->Seek(0, ADR_BEGIN);                  \
    }

  // grab the end of the filename so we can do end-of-string compares faster
  const char* filename_end = filename + strlen(filename);

  if (end_is(filename, filename_end, ".it") ||
      end_is(filename, filename_end, ".xm") ||
      end_is(filename, filename_end, ".s3m") ||
      end_is(filename, filename_end, ".mod")) {

    TRY_SOURCE(MODInputStream);

  } else if (end_is(filename, filename_end, ".wav")) {

    TRY_SOURCE(WAVInputStream);

  } else if (end_is(filename, filename_end, ".ogg")) {

    TRY_SOURCE(OGGInputStream);

  }

  // autodetect otherwise?
  // (in decreasing order of possibility of failure)
  TRY_SOURCE(MODInputStream);
  TRY_SOURCE(WAVInputStream);
  TRY_SOURCE(OGGInputStream);

  delete file;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
