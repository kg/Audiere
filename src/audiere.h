/**
 * Audiere Sound System
 * Version 1.2.0
 * (c) 2002 Chad Austin
 *
 * This API uses principles explained at
 * http://aegisknight.org/cppinterface.html
 *
 * This code licensed under the terms of the LGPL.  See license.txt.
 */

#ifndef AUDIERE_H
#define AUDIERE_H


#ifndef __cplusplus
  #error Audiere requires C++
#endif


// DLLs in Windows should use the standard calling convention
#if defined(WIN32) || defined(_WIN32)
  #define ADR_CALL __stdcall
#else
  #define ADR_CALL
#endif


#define ADR_FUNCTION(ret, name) extern "C" ret ADR_CALL name


namespace audiere {

  class DLLInterface {
  private:
    virtual void destroy() = 0;

  public:
    void operator delete(void* p) {
      if (p) {
        DLLInterface* i = static_cast<DLLInterface*>(p);
        i->destroy();
      }
    }
  };


  template<class Interface>
  class DLLImplementation : public Interface {
  public:
    virtual ~DLLImplementation() { }

    virtual void destroy() {
      delete this;
    }

    void operator delete(void* p) {
      ::operator delete(p);
    }
  };


  class File : public DLLInterface {
  public:
    enum SeekMode {
      BEGIN,
      CURRENT,
      END,
    };

    virtual int read(void* buffer, int size) = 0;
    virtual bool seek(int position, SeekMode mode) = 0;
    virtual int tell() = 0;
  };


  class SampleSource : public DLLInterface {
  public:
    virtual void getFormat(
      int& channel_count,
      int& sample_rate,
      int& bits_per_sample) = 0;
    virtual int read(int sample_count, void* buffer) = 0;
    virtual void reset() = 0;

    virtual bool isSeekable() = 0;
    virtual int getLength() = 0;
    virtual void setPosition(int position) = 0;
    virtual int getPosition() = 0;
  };


  class OutputStream : public DLLInterface {
  public:
    enum {
      MinimumVolume = 0,
      MaximumVolume = 255,
    };

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;
    virtual bool isPlaying() = 0;

    virtual void setVolume(int volume) = 0;
    virtual int getVolume() = 0;
  };


  enum SoundMode {
    STREAM,
    MEMORY_STREAM,
    BUFFER,
  };


  class Sound : public OutputStream {
  public:
    virtual void setRepeat(bool repeat) = 0;
    virtual bool getRepeat() = 0;

    virtual bool isSeekable() = 0;
    virtual int getLength() = 0;
    virtual void setPosition(int position) = 0;
    virtual int getPosition() = 0;
  };


  class AudioDevice : public DLLInterface {
  public:
    virtual void update() = 0;
    virtual OutputStream* openStream(SampleSource* source) = 0;
  };


  class OutputStream3D : public OutputStream {
  public:
    virtual void setPosition(double x, double y, double z) = 0;
    // ... ?
  };


  class AudioDevice3D : public AudioDevice {
    virtual OutputStream3D* openStream3D(SampleSource* source) = 0;
  };


  namespace hidden {
    ADR_FUNCTION(const char*, AdrGetVersion)();
    ADR_FUNCTION(AudioDevice*, AdrOpenDevice)(
      const char* name,
      const char* parameters);
    ADR_FUNCTION(AudioDevice3D*, AdrOpenDevice3D)(
      const char* name,
      const char* parameters);

    ADR_FUNCTION(SampleSource*, AdrOpenSampleSource)(const char* filename);
    ADR_FUNCTION(SampleSource*, AdrOpenSampleSourceFromFile)(File* file);

    ADR_FUNCTION(Sound*, AdrOpenSound)(
      AudioDevice* device,
      const char* filename,
      SoundMode mode);
    ADR_FUNCTION(Sound*, AdrOpenSoundFromFile)(
      AudioDevice* device,
      File* file,
      SoundMode mode);
  }


  inline const char* GetVersion() {
    return hidden::AdrGetVersion();
  }

  inline AudioDevice* OpenDevice(
    const char* name = 0,
    const char* parameters = 0)
  {
    return hidden::AdrOpenDevice(name, parameters);
  }

  inline AudioDevice3D* OpenDevice3D(
    const char* name = 0,
    const char* parameters = 0)
  {
    return hidden::AdrOpenDevice3D(name, parameters);
  }

  inline SampleSource* OpenSampleSource(const char* filename) {
    return hidden::AdrOpenSampleSource(filename);
  }

  inline SampleSource* OpenSampleSource(File* file) {
    return hidden::AdrOpenSampleSourceFromFile(file);
  }

  inline Sound* OpenSound(
    AudioDevice* device,
    const char* filename,
    SoundMode mode)
  {
    return hidden::AdrOpenSound(device, filename, mode);
  }

  inline Sound* OpenSound(
    AudioDevice* device,
    File* file,
    SoundMode mode)
  {
    return hidden::AdrOpenSoundFromFile(device, file, mode);
  }
}


#endif
