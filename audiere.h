#ifndef AUDIERE_H
#define AUDIERE_H


/* extern C */
#ifdef __cplusplus
extern "C" {
#endif


/* calling convention */
#ifdef _WIN32
#  define ADR_CALL __stdcall
#else
#  define ADR_CALL
#endif


/* boolean */
typedef int ADR_BOOL;
#define ADR_TRUE  1
#define ADR_FALSE 0


/* file callback types */
struct ADR_FileHandle;
typedef ADR_FileHandle* ADR_FILE;

typedef enum {
  ADR_BEGIN,   // stdio SEEK_SET
  ADR_CURRENT, // stdio SEEK_CUR
  ADR_END,     // stdio SEEK_END
} ADR_SEEK_TYPE;

typedef ADR_FILE (ADR_CALL *ADR_FILE_OPEN)(
  void* opaque,
  const char* filename);

typedef void (ADR_CALL *ADR_FILE_CLOSE)(
  ADR_FILE file);

typedef int (ADR_CALL *ADR_FILE_READ)(
  ADR_FILE file,
  void* buffer,
  int size);

typedef ADR_BOOL (ADR_CALL *ADR_FILE_SEEK)(
  ADR_FILE file,
  int offset,
  ADR_SEEK_TYPE type);

typedef int (ADR_CALL *ADR_FILE_TELL)(
  ADR_FILE file);


/* constants */
#define ADR_VOLUME_MIN (0)
#define ADR_VOLUME_MAX (255)


/* audiere opaque types */
typedef struct ADR_CONTEXT_ATTRimp* ADR_CONTEXT_ATTR;
typedef struct ADR_CONTEXTimp*      ADR_CONTEXT;
typedef struct ADR_STREAMimp*       ADR_STREAM;


/*
 * AdrGetVersion()
 *
 * Returns Audiere version string.
 *
 */
const char* ADR_CALL AdrGetVersion(void);


/*
 * AdrCreateContextAttr()
 *
 * Returns context attributes object with values set to defaults.
 *
 * output_device = ""
 * parameters    = ""
 * opaque        = 0
 * open          = default open
 * close         = default close
 * read          = default read
 * seek          = default seek
 * tell          = default tell
 *
 */
ADR_CONTEXT_ATTR ADR_CALL AdrCreateContextAttr(void);


/*
 * AdrDestroyContextAttr(attr)
 *
 * Destroys a context attributes object.
 *
 */
void ADR_CALL AdrDestroyContextAttr(
  ADR_CONTEXT_ATTR attr);


/*
 * output_device -- 
 *   string that represents the output device you want to use
 *   "" or "autodetect" will search for a good default device
 *   "null" is no sound
 *
 */
void ADR_CALL AdrContextAttrSetOutputDevice(
  ADR_CONTEXT_ATTR attr,
  const char* output_device);

/*
 * parameters --
 *   comma-delimited list of output device-specific parameters
 *   for example, "buffer=100,rate=44100"
 *
 */
void ADR_CALL AdrContextAttrSetParameters(
  ADR_CONTEXT_ATTR attr,
  const char* parameters);

/*
 * opaque --
 *   opaque handle passed into file I/O functions
 *
 */
void ADR_CALL AdrContextAttrSetOpaque(
  ADR_CONTEXT_ATTR attr,
  void* opaque);

/*
 * file callbacks
 *
 */
void ADR_CALL AdrContextAttrSetFileCallbacks(
  ADR_CONTEXT_ATTR attr,
  ADR_FILE_OPEN  open,
  ADR_FILE_CLOSE close,
  ADR_FILE_READ  read,
  ADR_FILE_SEEK  seek,
  ADR_FILE_TELL  tell);


/*
 * AdrCreateContext(attributes)
 *
 * Returns a new Audiere context or NULL if failure.
 *
 * attributes - set of context attributes, or NULL for defaults
 *
 */
ADR_CONTEXT ADR_CALL AdrCreateContext(
  ADR_CONTEXT_ATTR attr);


/*
 * AdrDestroyContext(context)
 *
 * Destroys a context, stopping the update thread and closing the output device.
 * Contexts aren't actually destroyed until all child streams are closed.
 *
 * context - the context to destroy, of course  ;)
 *
 */
void ADR_CALL AdrDestroyContext(
  ADR_CONTEXT context);


/*
 * AdrOpenStream(context, filename)
 *
 * Returns a new audio stream, or NULL if failure.
 *
 * context  - context within which to create the audio stream
 * filename - UTF-8 filename passed into file open callback
 *
 */
ADR_STREAM ADR_CALL AdrOpenStream(
  ADR_CONTEXT context,
  const char* filename);


/*
 * AdrCloseStream(stream)
 *
 * Closes a stream, halting audio output.
 *
 */
void ADR_CALL AdrCloseStream(
  ADR_STREAM stream);


/*
 * AdrPlayStream(stream)
 *
 * Begins playback of an audio stream.
 *
 */
void ADR_CALL AdrPlayStream(
  ADR_STREAM stream);


/*
 * AdrPauseStream(stream)
 *
 * Halts playback of an audio stream, but does not reset the position to the
 * beginning.
 *
 */
void ADR_CALL AdrPauseStream(
  ADR_STREAM stream);


/*
 * AdrResetStream(stream)
 *
 * Resets the current position within the sound file to the beginning.
 * This may be called at any time.
 *
 */
void ADR_CALL AdrResetStream(
  ADR_STREAM stream);


/*
 * AdrIsPlaying(stream)
 *
 * Returns ADR_TRUE if the stream is currently playing audio.
 *
 */
ADR_BOOL ADR_CALL AdrIsStreamPlaying(
  ADR_STREAM stream);


/*
 * AdrSetStreamRepeat(stream, repeat)
 *
 * If repeat is on and playback reaches the end of the stream, it will
 * automatically reset the stream and continue playback.
 */
void ADR_CALL AdrSetStreamRepeat(
  ADR_STREAM stream,
  ADR_BOOL repeat);


/*
 * AdrGetStreamRepeat(stream)
 *
 * Returns the repeat flag for the given stream.  Repeat defaults to false.
 *
 */
ADR_BOOL ADR_CALL AdrGetStreamRepeat(
  ADR_STREAM);


/*
 * AdrSetStreamVolume(stream, volume)
 *
 * Sets the stream volume.  Defaults to ADR_VOLUME_MAX.
 * ADR_VOLUME_MIN is silence.
 * ADR_VOLUME_MAX is full volume.
 *
 */
void ADR_CALL AdrSetStreamVolume(
  ADR_STREAM stream,
  int volume);


/*
 * AdrGetStreamVolume(stream)
 *
 * Returns the current stream volume.
 *
 */
int ADR_CALL AdrGetStreamVolume(
  ADR_STREAM stream);


#ifdef __cplusplus
}
#endif


// C++ convenience classes 
#ifdef __cplusplus

#include <exception>
#include <string>

namespace audiere {

  class Context;


  // STREAM

  class Stream {
  private:
    Stream(ADR_STREAM stream) {
      m_stream = stream;
    }

  public:
    ~Stream() {
      AdrCloseStream(m_stream);
    }

    void play() {
      AdrPlayStream(m_stream);
    }
    void pause() {
      AdrPauseStream(m_stream);
    }
    void reset() {
      AdrResetStream(m_stream);
    }
    bool isPlaying() {
      return (AdrIsStreamPlaying(m_stream) == ADR_TRUE);
    }
    void setRepeat(bool repeat) {
      AdrSetStreamRepeat(m_stream, repeat ? ADR_TRUE : ADR_FALSE);
    }
    bool getRepeat() {
      return (AdrGetStreamRepeat(m_stream) == ADR_TRUE);
    }
    void setVolume(int volume) {
      AdrSetStreamVolume(m_stream, volume);
    }
    int getVolume() {
      return AdrGetStreamVolume(m_stream);
    }
    
  private:
    ADR_STREAM m_stream;

    friend Context;
  };


  // CONTEXT ATTRIBUTES

  class ContextAttr {
  public:
    ContextAttr() {
      m_attr = AdrCreateContextAttr();
    }

    ~ContextAttr() {
      AdrDestroyContextAttr(m_attr);
    }

    void setOutputDevice(const char* device) {
      AdrContextAttrSetOutputDevice(m_attr, device);
    }
    void setParameters(const char* parameters) {
      AdrContextAttrSetParameters(m_attr, parameters);
    }
    void setOpaque(void* opaque) {
      AdrContextAttrSetOpaque(m_attr, opaque);
    }
    void setFileCallbacks(
        ADR_FILE_OPEN  open,
        ADR_FILE_CLOSE close,
        ADR_FILE_READ  read,
        ADR_FILE_SEEK  seek,
        ADR_FILE_TELL  tell) {
      
      AdrContextAttrSetFileCallbacks(m_attr, open, close, read, seek, tell);
    }

  private:
    ADR_CONTEXT_ATTR m_attr;

    friend Context* CreateContext(ContextAttr* attr);
  };


  // CONTEXT

  class Context {
  private:
    Context(ADR_CONTEXT context) {
      m_context = context;
    }

  public:
    ~Context() {
      AdrDestroyContext(m_context);
    }

    Stream* openStream(const char* filename) {
      ADR_STREAM stream = AdrOpenStream(m_context, filename);
      return (stream ? new Stream(stream) : 0);
    }

  private:
    ADR_CONTEXT m_context;

    friend Context* CreateContext(ContextAttr* attr);
  };


  // CONTEXT FACTORY
  inline Context* CreateContext(ContextAttr* attr = 0) {
    ADR_CONTEXT context = AdrCreateContext(attr ? attr->m_attr : 0);
    return (context ? new Context(context) : 0);
  }
}

#endif


#endif
