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

  class RefCounted {
  protected:
    /**
     * Protected so users of refcounted classes don't use std::auto_ptr
     * or the delete operator.
     *
     * Interfaces that derive from RefCounted should define an inline,
     * empty, protected destructor as well.
     */
    ~RefCounted() { }

  public:
    /**
     * Add a reference to the internal reference count.
     */
    virtual void ref() = 0;

    /**
     * Remove a reference from the internal reference count.  When this
     * reaches 0, the object is destroyed.
     */
    virtual void unref() = 0;
  };


  template<typename T>
  class RefPtr {
  public:
    RefPtr(T* ptr = 0) {
      m_ptr = 0;
      *this = ptr;
    }

    RefPtr(const RefPtr<T>& ptr) {
      m_ptr = 0;
      *this = ptr;
    }

    ~RefPtr() {
      if (m_ptr) {
        m_ptr->unref();
        m_ptr = 0;
      }
    }
 
    RefPtr<T>& operator=(T* ptr) {
      if (ptr != m_ptr) {
        if (m_ptr) {
          m_ptr->unref();
        }
        m_ptr = ptr;
        if (m_ptr) {
          m_ptr->ref();
        }
      }
      return *this;
    }

    RefPtr<T>& operator=(const RefPtr<T>& ptr) {
      *this = ptr.m_ptr;
      return *this;
    }

    T* operator->() {
      return m_ptr;
    }

    T& operator*() {
      return *m_ptr;
    }

    operator bool() {
      return (m_ptr != 0);
    }

    T* get() {
      return m_ptr;
    }

  private:
    T* m_ptr;
  };


  /**
   * A basic implementation of the RefCounted interface.  Derive
   * your implementations from RefImplementation<YourInterface>.
   */
  template<class Interface>
  class RefImplementation : public Interface {
  protected:
    RefImplementation() {
      m_ref_count = 0;
    }

    /**
     * So the implementation can put its destruction logic in the destructor,
     * as natural C++ code does.
     */
    virtual ~RefImplementation() { }

  public:
    void ref() {
      ++m_ref_count;
    }

    void unref() {
      if (--m_ref_count == 0) {
        delete this;
      }
    }

  private:
    int m_ref_count;
  };


  /**
   * Represents a random-access file, usually stored on a disk.  Files
   * are always binary: that is, they do no end-of-line
   * transformations.  File objects are roughly analogous to ANSI C
   * FILE* objects.
   */
  class File : public RefCounted {
  protected:
    ~File() { }

  public:
    /**
     * The different ways you can seek within a file.
     */
    enum SeekMode {
      BEGIN,
      CURRENT,
      END,
    };

    /**
     * Read size bytes from the file, storing them in buffer.
     *
     * @param buffer  buffer to read into
     * @param size    number of bytes to read
     *
     * @return  number of bytes successfully read
     */
    virtual int read(void* buffer, int size) = 0;

    /**
     * Jump to a new position in the file, using the specified seek
     * mode.  Remember: if mode is END, the position must be negative,
     * to seek backwards from the end of the file into its contents.
     * If the seek fails, the current position is undefined.
     *
     * @param position  position relative to the mode
     * @param mode      where to seek from in the file
     *
     * @return  true on success, false otherwise
     */
    virtual bool seek(int position, SeekMode mode) = 0;

    /**
     * Get current position within the file.
     *
     * @return  current position
     */
    virtual int tell() = 0;
  };


  /// Storage formats for sample data.
  enum SampleFormat {
    SF_U8,  ///< unsigned 8-bit integer [0,255]
    SF_S16, ///< signed 16-bit integer in host endianness [-32768,32767]
  };


  /**
   * Source of raw PCM samples.  Sample sources have an intrinsic format
   * (@see SampleFormat), sample rate, and number of channels.  They can
   * be read from or reset.
   *
   * Some sample sources are seekable.  Seekable sources have two additional
   * properties: length and position.  Length is read-only. 
   */
  class SampleSource : public RefCounted {
  protected:
    ~SampleSource() { }

  public:
    /**
     * Retrieve the number of channels, sample rate, and sample format of
     * the sample source.
     */
    virtual void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format) = 0;

    /**
     * Read frame_count samples into buffer.  buffer must be at least
     * |frame_count * GetSampleSize(format) * channel_count| bytes long.
     *
     * @param frame_count  number of frames to read
     * @param buffer       buffer to store samples in
     *
     * @return  number of frames actually read
     */
    virtual int read(int frame_count, void* buffer) = 0;

    /**
     * Reset the sample source.  This has the same effect as setPosition(0)
     * on a seekable source.  On an unseekable source, it resets all internal
     * state to the way it was when the source was first created.
     */
    virtual void reset() = 0;

    /**
     * @return  true if the stream is seekable, false otherwise
     */
    virtual bool isSeekable() = 0;

    /**
     * @return  number of frames in the stream, or 0 if the stream is not
     *          seekable
     */
    virtual int getLength() = 0;
    
    /**
     * Sets the current position within the sample source.  If the stream
     * is not seekable, this method does nothing.
     *
     * @param position  current position in frames
     */
    virtual void setPosition(int position) = 0;

    /**
     * Returns the current position within the sample source.
     *
     * @return  current position in frames
     */
    virtual int getPosition() = 0;
  };


  /**
   * A connection to an audio device.  Multiple output streams are
   * mixed by the audio device to produce the final waveform that the
   * user hears.
   *
   * Each output stream can be independently played and stopped.  They
   * also each have a volume from 0.0 (silence) to 1.0 (maximum volume).
   */
  class OutputStream : public RefCounted {
  protected:
    ~OutputStream() { }

  public:
    /**
     * Start playback of the output stream.  If the stream is already
     * playing, this does nothing.
     */
    virtual void play() = 0;

    /**
     * Stop playback of the output stream.  If the stream is already
     * stopped, this does nothing.
     */
    virtual void stop() = 0;

    /**
     * @return  true if the output stream is playing, false otherwise
     */
    virtual bool isPlaying() = 0;

    /**
     * Reset the output stream's internal buffer, causing it to buffer new
     * samples from its sample source.
     *
     * On some output streams, this operation can be moderately slow, as up to
     * several seconds of PCM buffer must be refilled.
     */
    virtual void reset() = 0;

    /**
     * Set whether the output stream should repeat.
     *
     * @param repeat  true if the stream should repeat, false otherwise
     */
    virtual void setRepeat(bool repeat) = 0;

    /**
     * @return  true if the stream is repeating
     */
    virtual bool getRepeat() = 0;

    /**
     * Sets the stream's volume.
     *
     * @param  volume  0.0 = silence, 1.0 = maximum volume (default)
     */
    virtual void setVolume(float volume) = 0;

    /**
     * Gets the current volume.
     *
     * @return  current volume of the output stream
     */
    virtual float getVolume() = 0;

    /**
     * Set current pan.
     *
     * @param pan  -1.0 = left, 0.0 = center (default), 1.0 = right
     */
    virtual void setPan(float pan) = 0;

    /**
     * Get current pan.
     */
    virtual float getPan() = 0;

    /**
     * @return  true if the stream is seekable, false otherwise
     */
    virtual bool isSeekable() = 0;

    /**
     * @return  number of frames in the stream, or 0 if the stream is not
     *          seekable
     */
    virtual int getLength() = 0;
    
    /**
     * Sets the current position within the sample source.  If the stream
     * is not seekable, this method does nothing.
     *
     * @param position  current position in frames
     */
    virtual void setPosition(int position) = 0;

    /**
     * Returns the current position within the sample source.
     *
     * @return  current position in frames
     */
    virtual int getPosition() = 0;
  };


  /**
   * AudioDevice represents a device on the system which is capable
   * of opening and mixing multiple output streams.  In Windows,
   * DirectSound is such a device.
   *
   * This interface is synchronized.  update() and openStream() may
   * be called on different threads.
   */
  class AudioDevice : public RefCounted {
  protected:
    ~AudioDevice() { }

  public:
    /**
     * Tell the device to do any internal state updates.  Some devices
     * update on an internal thread.  If that is the case, this method
     * does nothing.
     */
    virtual void update() = 0;

    /**
     * Open an output stream with a given sample source.  If the sample
     * source ever runs out of data, the output stream automatically stops
     * itself.
     *
     * The output stream takes ownership of the sample source, even if
     * opening the output stream fails (in which case the source is
     * immediately deleted).
     *
     * @param  source  the source used to feed the output stream with samples
     *
     * @return  new output stream if successful, 0 if failure
     */
    virtual OutputStream* openStream(SampleSource* source) = 0;

    /**
     * Open a single buffer with the specified PCM data.  This is sometimes
     * more efficient than streaming and works on a larger variety of audio
     * devices.  In some implementations, this may download the audio data
     * to the sound card's memory itself.
     *
     * @param samples  Buffer containing sample data.  openBuffer() does
     *                 not take ownership of the memory.  The application
     *                 is responsible for freeing it.  There must be at
     *                 least |frame_count * channel_count *
     *                 GetSampleSize(sample_format)| bytes in the buffer.
     *
     * @param frame_count  Number of frames in the buffer.
     *
     * @param channel_count  Number of audio channels.  1 = mono, 2 = stereo.
     *
     * @param sample_rate  Number of samples per second.
     *
     * @param sample_format  Format of samples in buffer.
     *
     * @return  new output stream if successful, 0 if failure
     */
    virtual OutputStream* openBuffer(
      void* samples,
      int frame_count,
      int channel_count,
      int sample_rate,
      SampleFormat sample_format) = 0;
  };


  /// PRIVATE API - for internal use only
  namespace hidden {

    // these are extern "C" so we don't mangle the names

    ADR_FUNCTION(const char*, AdrGetVersion)();
    ADR_FUNCTION(int, AdrGetSampleSize)(SampleFormat format);

    ADR_FUNCTION(AudioDevice*, AdrOpenDevice)(
      const char* name,
      const char* parameters);

    ADR_FUNCTION(SampleSource*, AdrOpenSampleSource)(const char* filename);
    ADR_FUNCTION(SampleSource*, AdrOpenSampleSourceFromFile)(File* file);
    ADR_FUNCTION(SampleSource*, AdrCreateTone)(double frequency);

    ADR_FUNCTION(OutputStream*, AdrOpenSound)(
      AudioDevice* device,
      SampleSource* source);
  }


  /* PUBLIC API */


  /**
   * Return the Audiere version string.
   *
   * @return  Audiere version information
   */
  inline const char* GetVersion() {
    return hidden::AdrGetVersion();
  }

  /**
   * Get the size of a sample in a specific sample format.
   * This is commonly used to determine how many bytes a chunk of
   * PCM data will take.
   *
   * @return  Number of bytes a single sample in the specified format
   *          takes.
   */
  inline int GetSampleSize(SampleFormat format) {
    return hidden::AdrGetSampleSize(format);
  }

  /**
   * Open a new audio device. If name or parameters are not specified,
   * defaults are used. Each platform has its own set of audio devices.
   * Every platform supports the "null" audio device.
   *
   * @param  name  name of audio device that should be used
   * @param  parameters  comma delimited list of audio-device parameters;
   *                     for example, "buffer=100,rate=44100"
   *
   * @return  new audio device object if OpenDevice succeeds, and 0 in case
   *          of failure
   */
  inline AudioDevice* OpenDevice(
    const char* name = 0,
    const char* parameters = 0)
  {
    return hidden::AdrOpenDevice(name, parameters);
  }

  /**
   * Create a streaming sample source from a sound file.  This factory simply
   * opens a default file from the system filesystem and calls
   * OpenSampleSource(File*).
   *
   * @see OpenSampleSource(File*)
   */
  inline SampleSource* OpenSampleSource(const char* filename) {
    return hidden::AdrOpenSampleSource(filename);
  }

  /**
   * Opens a sample source from the specified file object.  If the sound file
   * cannot be opened, this factory function returns 0.
   *
   * @note  Some sound files support seeking, while some don't.
   *
   * @param file  File object from which to open the decoder
   *
   * @return  new SampleSource if OpenSampleSource succeeds, 0 otherwise
   */
  inline SampleSource* OpenSampleSource(File* file) {
    return hidden::AdrOpenSampleSourceFromFile(file);
  }

  /**
   * Create a tone sample source with the specified frequency.
   *
   * @param  frequency  Frequency of the tone in Hz.
   *
   * @return  tone sample source
   */
  inline SampleSource* CreateTone(double frequency) {
    return hidden::AdrCreateTone(frequency);
  }

  /**
   * Try to open a sound buffer using the specified AudioDevice and
   * sample source.  If the specified sample source is seekable, it
   * loads it into memory and uses AudioDevice::openBuffer to create
   * the output stream.  If the stream is not seekable, it uses
   * AudioDevice::openStream to create the output stream.
   *
   * @param device  AudioDevice in which to open the output stream.
   *
   * @param source  SampleSource used to generate samples for the sound
   *                object.  OpenSound takes ownership of source, even
   *                if it returns 0.  (In that case, OpenSound immediately
   *                deletes the SampleSource.)
   *
   * @return  new output stream if successful, 0 otherwise
   */
  inline OutputStream* OpenSound(AudioDevice* device, SampleSource* source) {
    return hidden::AdrOpenSound(device, source);
  }

  /**
   * Calls OpenSound(AudioDevice*, SampleSource*) with a sample source
   * created via OpenSampleSource(const char*).
   */
  inline OutputStream* OpenSound(AudioDevice* device, const char* filename) {
    return OpenSound(device, OpenSampleSource(filename));
  }

  /**
   * Calls OpenSound(AudioDevice*, SampleSource*) with a sample source
   * created via OpenSampleSource(File* file).
   */
  inline OutputStream* OpenSound(AudioDevice* device, File* file) {
    return OpenSound(device, OpenSampleSource(file));
  }
}


#endif
