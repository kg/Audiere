#include <memory>
#include <string.h>
#include "debug.h"
#include "default_file.h"
#ifndef NO_MIKMOD
#include "input_mod.h"
#endif
#include "input_mp3.h"
#include "input_ogg.h"
#include "input_wav.h"
#include "internal.h"
#include "utility.h"


namespace audiere {

  bool end_is(const char* begin, const char* ext) {
    const char* end = begin + strlen(begin);
    int ext_length = strlen(ext);
    if (ext_length > end - begin) {
      return false;
    } else {
      return (strcmp_case(end - ext_length, ext) == 0);
    }
  }


  template<typename T>
  static T* TryInputStream(File* file) {

    // initialize should never close the file

    T* source = new T();
    if (source->initialize(file)) {
      return source;
    } else {
      delete source;
      return 0;
    }
  }


  /**
   * The internal implementation of OpenSampleSource.
   *
   * @param raw_file  the file to load from.  cannot be 0.
   * @param filename  the name of the file, or 0 if it is not available
   */
  SampleSource* OpenSource(File* raw_file, const char* filename) {
    ADR_GUARD("OpenSource");

    RefPtr<File> file(raw_file);

    #define TRY_SOURCE(source_type) {                                 \
      source_type* source = TryInputStream<source_type>(file.get());  \
      if (source) {                                                   \
        return source;                                                \
      } else {                                                        \
        file->seek(0, File::BEGIN);                                   \
      }                                                               \
    }

    // if filename is available, use it as a hint
    if (filename) {
      if (end_is(filename, ".wav")) {

        TRY_SOURCE(WAVInputStream);

      } else if (end_is(filename, ".ogg")) {

        TRY_SOURCE(OGGInputStream);

#ifndef NO_MIKMOD
      } else if (end_is(filename, ".it") ||
          end_is(filename, ".xm") ||
          end_is(filename, ".s3m") ||
          end_is(filename, ".mod")) {

        TRY_SOURCE(MODInputStream);
#endif
      } else if (end_is(filename, ".mp3")) {

        TRY_SOURCE(MP3InputStream);

      }
    }

    // autodetect otherwise, in decreasing order of possibility of failure
#ifndef NO_MIKMOD
    TRY_SOURCE(MODInputStream);
#endif
    TRY_SOURCE(WAVInputStream);
    TRY_SOURCE(OGGInputStream);
    TRY_SOURCE(MP3InputStream);

    return 0;
  }


  ADR_EXPORT(SampleSource*, AdrOpenSampleSource)(const char* filename) {
    if (!filename) {
      return 0;
    }
    File* file = OpenDefaultFile(filename);
    if (!file) {
      return 0;
    }
    return OpenSource(file, filename);
  }


  ADR_EXPORT(SampleSource*, AdrOpenSampleSourceFromFile)(File* file) {
    if (!file) {
      return 0;
    }
    return OpenSource(file, 0);
  }

}
