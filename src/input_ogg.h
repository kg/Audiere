#ifndef INPUT_OGG_H
#define INPUT_OGG_H


#include <vorbis/vorbisfile.h>
#include "audiere.h"
#include "utility.h"


namespace audiere {

  class OGGInputStream : public RefImplementation<SampleSource> {
  public:
    OGGInputStream();
    ~OGGInputStream();

    bool initialize(File* file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int ADR_CALL read(int frame_count, void* buffer);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

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
