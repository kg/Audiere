#include <memory>
#include <string.h>
#include "debug.hpp"
#include "default_file.hpp"
#include "input_mod.hpp"
#include "input_ogg.hpp"
#include "input_wav.hpp"
#include "internal.hpp"
#include "utility.hpp"


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
   * @param file      the file to load from.  cannot be 0.
   * @param filename  the name of the file, or 0 if it is not available
   */
  SampleSource* OpenSource(File* file, const char* filename) {
    ADR_GUARD("OpenSource");

    #define TRY_SOURCE(source_type) {                           \
      source_type* source = TryInputStream<source_type>(file);  \
      if (source) {                                             \
        return source;                                          \
      } else {                                                  \
        file->seek(0, File::BEGIN);                             \
      }                                                         \
    }

    // if filename is available, use it as a hint
    if (filename) {
      if (end_is(filename, ".it") ||
          end_is(filename, ".xm") ||
          end_is(filename, ".s3m") ||
          end_is(filename, ".mod")) {

        TRY_SOURCE(MODInputStream);

      } else if (end_is(filename, ".wav")) {

        TRY_SOURCE(WAVInputStream);

      } else if (end_is(filename, ".ogg")) {

        TRY_SOURCE(OGGInputStream);

      }
    }

    // autodetect otherwise, in decreasing order of possibility of failure
    TRY_SOURCE(MODInputStream);
    TRY_SOURCE(WAVInputStream);
    TRY_SOURCE(OGGInputStream);

    delete file;
    return 0;
  }


  ADR_EXPORT(SampleSource*, AdrOpenSampleSource)(const char* filename) {
    if (!filename) {
      return 0;
    }
    std::auto_ptr<File> file(OpenDefaultFile(filename));
    return OpenSource(file.get(), filename);
  }


  ADR_EXPORT(SampleSource*, AdrOpenSampleSourceFromFile)(File* file) {
    if (!file) {
      return 0;
    }
    return OpenSource(file, 0);
  }

}
