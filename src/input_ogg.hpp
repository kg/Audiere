#ifndef INPUT_OGG_HPP
#define INPUT_OGG_HPP


#include <vorbis/vorbisfile.h>
#include "audiere.h"
#include "utility.hpp"


namespace audiere {

  class OGGInputStream : public UnseekableSource {
  public:
    OGGInputStream();
    ~OGGInputStream();

    bool initialize(File* file);

    void getFormat(
      int& channel_count,
      int& sample_rate,
      int& bits_per_sample);
    int read(int sample_count, void* samples);
    void reset();

  private:
    static size_t FileRead(void* buffer, size_t size, size_t n, void* opaque);
    static int    FileSeek(void* opaque, ogg_int64_t offset, int whence);
    static int    FileClose(void* opaque);
    static long   FileTell(void* opaque);

  private:
    File* m_file;

    OggVorbis_File m_vorbis_file;
  //  bool           m_eof;

    int m_channel_count;
    int m_sample_rate;
    int m_bits_per_sample;
  };

}

#endif
