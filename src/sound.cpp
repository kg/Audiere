#include <memory>
#include "audiere.h"
#include "internal.h"


namespace audiere {

  ADR_EXPORT(Sound*, AdrOpenSound)(
    AudioDevice* device,
    SampleSource* source,
    SoundMode mode)
  {
    if (!device || !source) {
      return 0;
    }

    return 0;
  }

}
