#ifndef INPUT_OGG_H
#define INPUT_OGG_H


#include <vorbis/vorbisfile.h>
#include "audiere.h"
#include "utility.h"


namespace audiere {

  class OGGInputStream : public UnseekableSource {
  public:
    OGGInputStream();
    ~OGGInputStream();

    bool initialize(File* file);

    void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int read(int sample_count, void* samples);
    void reset();

  private:
    static size_t FileRead(void* buffer, size_t size, size_t n, void* opaque);
    static int    FileSeek(void* opaque, ogg_int64_t offset, int whence);
    static int    FileClose(void* opaque);
    static long   FileTell(void* opaque);

  private:
    RefPtr<File> m_file;

    OggVorbis_File m_vorbis_file;

    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;
  };

}

#endif
