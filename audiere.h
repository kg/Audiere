#ifndef AUDIERE_H
#define AUDIERE_H


/* extern */
#ifdef __cplusplus
  #define ADR_EXTERN_C extern "C"
#else
  #define ADR_EXTERN_C extern
#endif


/* calling convention */
#define ADR_CALL __stdcall

/* file callback types */
typedef void* (ADR_CALL *ADR_FILE_OPEN)(void* opaque, const char* filename);
typedef void  (ADR_CALL *ADR_FILE_CLOSE)(void* file);
typedef int   (ADR_CALL *ADR_FILE_READ)(void* file, void* buffer, int size);
typedef int   (ADR_CALL *ADR_FILE_SEEK)(void* file, int destination);
typedef int   (ADR_CALL *ADR_FILE_TELL)(void* file);


/* constants */
#define ADR_VOLUME_MIN (0)
#define ADR_VOLUME_MAX (255)
#define ADR_PAN_LEFT   (-255)
#define ADR_PAN_CENTER (0)
#define ADR_PAN_RIGHT  (255)


/* audiere opaque types */
typedef void* ADR_CONTEXT;
typedef void* ADR_STREAM;


/* boolean */
typedef int ADR_BOOL;
#define ADR_TRUE  1
#define ADR_FALSE 0



/*
 * AdrGetVersion()
 *
 * Returns Audiere version string
 *
 */
ADR_EXTERN_C const char* ADR_CALL AdrGetVersion(void);


/*
 * AdrCreateContext(output_device, parameters,
 *                  opaque, open, close, read, seek, tell)
 *
 * Returns a new Audiere context or NULL if failure.
 *
 * output_device - string that represents the output device you want to use
 *                 "autodetect" will just give you a device
 *                 "null" is no sound
 * parameters    - comma-delimited list of output device-specific parameters
 *                 for example, "buffer=100,rate=44100"
 * opaque        - opaque handle passed into file I/O functions
 * open          - open file callback
 * close         - close file callback
 * read          - read from file callback
 * seek          - seek in file callback
 * tell          - get position in file callback
 *
 */
ADR_EXTERN_C ADR_CONTEXT ADR_CALL AdrCreateContext(
  const char* output_device,
  const char* parameters,
  void* opaque,
  ADR_FILE_OPEN  open,
  ADR_FILE_CLOSE close,
  ADR_FILE_READ  read,
  ADR_FILE_SEEK  seek,
  ADR_FILE_TELL  tell);


/*
 * AdrDestroyContext(context)
 *
 * Destroys a context, stopping the update thread and closing the output device.
 * Contexts aren't actually destroyed until all child streams are closed.
 *
 * context - the context to destroy, of course  ;)
 *
 */
ADR_EXTERN_C void ADR_CALL AdrDestroyContext(
  ADR_CONTEXT context);


/*
 * AdrOpenStream(context, filename)
 *
 * Returns a new audio stream, or NULL if failure.
 *
 * context  - context within which to create the audio stream
 * filename - name passed into file open callback
 *
 */
ADR_EXTERN_C ADR_STREAM ADR_CALL AdrOpenStream(
  ADR_CONTEXT context,
  const char* filename);


/*
 * AdrCloseStream(stream)
 *
 * Closes a stream, halting audio output.
 *
 */
ADR_EXTERN_C void ADR_CALL AdrCloseStream(
  ADR_STREAM stream);


/*
 * AdrPlayStream(stream)
 *
 * Begins playback of an audio stream.
 *
 */
ADR_EXTERN_C void ADR_CALL AdrPlayStream(
  ADR_STREAM stream);


/*
 * AdrPauseStream(stream)
 *
 * Halts playback of an audio stream, but does not reset the position to the
 * beginning.
 *
 */
ADR_EXTERN_C void ADR_CALL AdrPauseStream(
  ADR_STREAM stream);


/*
 * AdrResetStream(stream)
 *
 * Resets the current position within the sound file to the beginning.
 * This may be called at any time.
 *
 */
ADR_EXTERN_C void ADR_CALL AdrResetStream(
  ADR_STREAM stream);


/*
 * AdrIsPlaying(stream)
 *
 * Returns ADR_TRUE if the stream is currently playing audio.
 *
 */
ADR_EXTERN_C ADR_BOOL ADR_CALL AdrIsStreamPlaying(
  ADR_STREAM stream);


/*
 * AdrSetStreamRepeat(stream, repeat)
 *
 * If repeat is on and playback reaches the end of the stream, it will
 * automatically reset the stream and continue playback.
 */
ADR_EXTERN_C void ADR_CALL AdrSetStreamRepeat(
  ADR_STREAM stream,
  ADR_BOOL repeat);


/*
 * AdrGetStreamRepeat(stream)
 *
 * Returns the repeat flag for the given stream.  Repeat defaults to false.
 *
 */
ADR_EXTERN_C ADR_BOOL ADR_CALL AdrGetStreamRepeat(
  ADR_STREAM);


/*
 * AdrSetStreamVolume(stream, volume)
 *
 * Sets the stream volume.  Defaults to ADR_VOLUME_MAX.
 * ADR_VOLUME_MIN is silence.
 * ADR_VOLUME_MAX is full volume.
 *
 */
ADR_EXTERN_C void ADR_CALL AdrSetStreamVolume(
  ADR_STREAM stream,
  int volume);


/*
 * AdrGetStreamVolume(stream)
 *
 * Returns the current stream volume.
 *
 */
ADR_EXTERN_C int ADR_CALL AdrGetStreamVolume(
  ADR_STREAM stream);


/*
 * AdrSetStreamPan(stream, pan)
 *
 * Sets the stream pan.  Defaults to ADR_PAN_CENTER.
 * ADR_PAN_LEFT is 100% left.
 * ADR_PAN_CENTER (0) is centered.
 * ADR_PAN_RIGHT is 100% right.
 *
 */
ADR_EXTERN_C void ADR_CALL AdrSetStreamPan(
  ADR_STREAM stream,
  int pan);


/*
 * AdrGetStreamPan(stream)
 *
 * Returns the current stream pan.
 *
 */
ADR_EXTERN_C int ADR_CALL AdrGetStreamPan(
  ADR_STREAM stream);


#endif
