#ifndef ACOUSTIQUE_H
#define ACOUSTIQUE_H


/* extern */
#ifdef __cplusplus
  #define ACQ_EXTERN_C extern "C"
#else
  #define ACQ_EXTERN_C extern
#endif


/* calling convention */
#ifdef _WIN32
  #define ACQ_CALL __stdcall
#else
  #define ACQ_CALL
#endif


/* stream callback types */
typedef int  (ACQ_CALL *ACQ_READ_CALLBACK)(
  void* opaque,
  void* bytes,
  int byte_count);
typedef void (ACQ_CALL *ACQ_RESET_CALLBACK)(
  void* opaque);


/* ACQ_BOOL */
typedef int ACQ_BOOL;
#define ACQ_TRUE  1
#define ACQ_FALSE 0


/* ACQ_STREAM */
typedef void* ACQ_STREAM;


/* stream format definitions */
enum {
  ACQ_STREAM_AUTODETECT = 0x0000,

  ACQ_STREAM_OGG = 0x0101,
  ACQ_STREAM_MP3 = 0x0102,
  ACQ_STREAM_WAV = 0x0103,
  ACQ_STREAM_MOD = 0x0104,
};


/*
 * AcqGetVersion()
 *
 * Returns Acoustique version string.
 *
 */

ACQ_EXTERN_C const char* ACQ_CALL AcqGetVersion(void);


/*
 * AcqOpenStream(opaque, read, seek, tell, stream_format, pcm_format)
 *
 * Returns a new Acoustique stream or NULL if failure.
 *
 * opaque        - handle passed into stream callbacks
 * read          - read stream callback
 * reset         - reset stream callback
 * stream_format - stream format type
 *
 */

ACQ_EXTERN_C ACQ_STREAM ACQ_CALL AcqOpenStream(
  void* opaque,
  ACQ_READ_CALLBACK read,
  ACQ_RESET_CALLBACK reset,
  int stream_format);


/*
 * AcqCloseStream(stream)
 *
 * Closes the Acoustique stream.
 *
 */

ACQ_EXTERN_C void ACQ_CALL AcqCloseStream(
  ACQ_STREAM stream);


/*
 * AcqGetStreamInformation(stream, num_channels, bits_per_sample, sample_rate)
 *
 * Returns stream information
 *
 */

ACQ_EXTERN_C void ACQ_CALL AcqGetStreamInformation(
  ACQ_STREAM stream,
  int* channel_count,
  int* bits_per_sample,
  int* sample_rate);


/*
 * AcqReadStream(stream, num_samples)
 *
 * Attempts to read sample_count samples from the stream.  Returns
 * actual number of samples read.  Copies sample data into 'samples'.
 *
 */

ACQ_EXTERN_C int ACQ_CALL AcqReadStream(
  ACQ_STREAM stream,
  void* samples,
  int sample_count);


/*
 * AcqResetStream(stream)
 *
 * Resets a stream to the beginning.
 *
 * stream - handle to the stream
 *
 */

ACQ_EXTERN_C ACQ_BOOL ACQ_CALL AcqResetStream(
  ACQ_STREAM stream);



#endif
