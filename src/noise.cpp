#include <stdlib.h>
#include "internal.h"
#include "utility.h"
#include "types.h"

namespace audiere {

  class WhiteNoise : public UnseekableSource {
  public:
    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format)
    {
      channel_count = 1;
      sample_rate   = 44100;
      sample_format = SF_S16;
    }

    int ADR_CALL read(int frame_count, void* buffer) {
      s16* out = (s16*)buffer;
      for (int i = 0; i < frame_count; ++i) {
        *out++ = (rand() % 65536 - 32768);
      }
      return frame_count;
    }

    void ADR_CALL reset() {
    }

  private:
  };


  ADR_EXPORT(SampleSource*, AdrCreateWhiteNoise)() {
    return new WhiteNoise();
  }

}
