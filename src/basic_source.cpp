#include "basic_source.h"
#include "debug.h"
#include "utility.h"


namespace audiere {

  BasicSource::BasicSource() {
    m_repeat = false;
  }

  int
  BasicSource::read(int frame_count, void* buffer) {
    if (m_repeat) {

      // calculate this on every read... shouldn't be too much overhead
      int channel_count, sample_rate;
      SampleFormat sample_format;
      getFormat(channel_count, sample_rate, sample_format);
      const int frame_size = channel_count * GetSampleSize(sample_format);

      // the main read loop:
      u8* out = (u8*)buffer;
      int frames_left = frame_count;
      while (frames_left > 0) {

        // read some frames
        int frames_read = doRead(frames_left, out);

        // if we couldn't read anything, reset the stream and try again
        if (frames_read == 0) {
          reset();
          frames_read = doRead(frames_left, out);
        }

        // if we still can't read anything, we're done
        if (frames_read == 0) {
          ADR_LOG("Can't read any samples even after reset");
          break;
        }

        frames_left -= frames_read;
        out += frames_read * frame_size;
      }

      return frame_count - frames_left;

    } else {

      return doRead(frame_count, buffer);

    }
  }

}
