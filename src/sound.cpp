#include <memory>
#include "audiere.h"
#include "internal.h"


namespace audiere {

  /**
   * Internal implementation of OpenSound.
   *
   * @return  0 if sound could not be created.
   *
   * @pre   device and source shall not be 0.
   * @post  DoOpenSound takes ownership of the sample source.
   */
  Sound* DoOpenSound(
    AudioDevice* device,
    SampleSource* source,
    SoundMode mode)
  {
    delete source;
    return 0;
  }


  ADR_EXPORT(Sound*, AdrOpenSound)(
    AudioDevice* device,
    const char* filename,
    SoundMode mode)
  {
    if (!device || !filename) {
      return 0;
    }

    SampleSource* source = OpenSampleSource(filename);
    if (!source) {
      return 0;
    }

    return DoOpenSound(device, source, mode);
  }


  ADR_EXPORT(Sound*, AdrOpenSoundFromFile)(
    AudioDevice* device,
    File* file,
    SoundMode mode)
  {
    if (!device || !file) {
      return 0;
    }

    SampleSource* source = OpenSampleSource(file);
    if (!source) {
      return 0;
    }

    return DoOpenSound(device, source, mode);
  }

}
