#include "input_flac.h"
#include "types.h"
#include "utility.h"


namespace audiere {


  class MyFLACDecoder : public FLAC::Decoder::SeekableStream {
  public:
    MyFLACDecoder(FLACInputStream* stream) {
      m_stream = stream;
    }

    File* getFile() {
      return m_stream->m_file.get();
    }

    FLAC__SeekableStreamDecoderReadStatus read_callback(
      FLAC__byte buffer[],
      unsigned *bytes)
    {
      *bytes = getFile()->read(buffer, *bytes);
      if (*bytes == 0) {
        return FLAC__SEEKABLE_STREAM_DECODER_READ_STATUS_ERROR;
      } else {
        return FLAC__SEEKABLE_STREAM_DECODER_READ_STATUS_OK;
      }
    }

    FLAC__SeekableStreamDecoderSeekStatus seek_callback(
      FLAC__uint64 absolute_byte_offset)
    {
      if (getFile()->seek(absolute_byte_offset, File::BEGIN)) {
        return FLAC__SEEKABLE_STREAM_DECODER_SEEK_STATUS_OK;
      } else {
        return FLAC__SEEKABLE_STREAM_DECODER_SEEK_STATUS_ERROR;
      }
    }

    FLAC__SeekableStreamDecoderTellStatus tell_callback(
      FLAC__uint64* absolute_byte_offset)
    {
      *absolute_byte_offset = getFile()->tell();
      return FLAC__SEEKABLE_STREAM_DECODER_TELL_STATUS_OK;
    }

    FLAC__SeekableStreamDecoderLengthStatus length_callback(
      FLAC__uint64* stream_length)
    {
      *stream_length = GetFileLength(getFile());
      return FLAC__SEEKABLE_STREAM_DECODER_LENGTH_STATUS_OK;
    }

    bool eof_callback() {
      return (getFile()->tell() == GetFileLength(getFile()));
    }

    FLAC__StreamDecoderWriteStatus write_callback(
      const FLAC__Frame* frame,
      const FLAC__int32* const buffer[])
    {
      return m_stream->write(frame, buffer);
    }

    void metadata_callback(const FLAC__StreamMetadata *metadata) {
      if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        m_stream->m_length = metadata->data.stream_info.total_samples;
      }
    }

    void error_callback(FLAC__StreamDecoderErrorStatus status) {
      // don't handle any errors
    }

  private:
    FLACInputStream* m_stream;
  };




  FLACInputStream::FLACInputStream() {
    m_decoder = 0;

    m_channel_count = 0;
    m_sample_rate   = 0;
    m_sample_format = SF_S16;

    m_length = 0;
    m_position = 0;
  }


  FLACInputStream::~FLACInputStream() {
    delete m_decoder;
  }


  bool
  FLACInputStream::initialize(File* file) {
    m_file = file;

    // initialize the decoder
    m_decoder = new MyFLACDecoder(this);
    int status = m_decoder->init();
    if (status != FLAC__SEEKABLE_STREAM_DECODER_OK) {
      delete m_decoder;
      m_decoder = 0;
      m_file = 0;
      return false;
    }

    // process one frame so we can do something!
    if (!m_decoder->process_until_end_of_metadata()) {
      delete m_decoder;
      m_decoder = 0;
      m_file = 0;
      return false;
    }

    // process one frame so we can do something!
    if (!m_decoder->process_single()) {
      delete m_decoder;
      m_decoder = 0;
      m_file = 0;
      return false;
    }

    // get info about the flac file
    m_channel_count = m_decoder->get_channels();
    m_sample_rate   = m_decoder->get_sample_rate();
    int bps         = m_decoder->get_bits_per_sample();
    if (bps == 16) {
      m_sample_format = SF_S16;
    } else {
      m_sample_format = SF_U8;
    }

    return true;
  }


  void
  FLACInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
  }


  int
  FLACInputStream::read(int frame_count, void* samples) {
    const int frame_size = m_channel_count * GetSampleSize(m_sample_format);
    u8* out = (u8*)samples;
    
    // we keep reading till we finish topping up!
    int frames_read = 0;
    while (frames_read < frame_count) {
      
      // if the buffer is empty, ask FLAC to fill it p
      if (m_buffer.getSize() < frame_size) {
        if (!m_decoder->process_single()) {
          return frames_read;
        }

        // if the buffer still has a size of 0, we are probably at the
        // end of the stream
        if (m_buffer.getSize() < frame_size) {
          return frames_read;
        }
      }

      // read what we've got!
      const int to_read = std::min(
        frame_count - frames_read,
        m_buffer.getSize() / frame_size);
      m_buffer.read(out, to_read * frame_size);
      out += to_read * frame_size;
      frames_read += to_read;
    }

    return frames_read;
  }


  void
  FLACInputStream::reset() {
    m_file->seek(0, File::BEGIN);
    m_decoder->seek_absolute(0);
    m_position = 0;
    m_buffer.clear();
  }


  bool
  FLACInputStream::isSeekable() {
    return (m_length != 0);
  }


  int
  FLACInputStream::getLength() {
    return m_length;
  }


  void
  FLACInputStream::setPosition(int position) {
    if (m_decoder->seek_absolute(position)) {
      m_position = position;
    }
  }


  int
  FLACInputStream::getPosition() {
    int bytes_per_frame = m_channel_count * GetSampleSize(m_sample_format);
    return m_position - (m_buffer.getSize() / bytes_per_frame);
  }


  FLAC__StreamDecoderWriteStatus
  FLACInputStream::write(
    const FLAC__Frame* frame,
    const FLAC__int32* const buffer[])
  {
    int channel_count = frame->header.channels;
    int samples_per_channel = frame->header.blocksize;
    int bytes_per_sample = frame->header.bits_per_sample / 8;
    int total_size = channel_count * samples_per_channel * bytes_per_sample;

    m_multiplexer.ensureSize(total_size);

    // do the multiplexing/interleaving
    if (bytes_per_sample == 1) {
      u8* out = (u8*)m_multiplexer.get();
      for (int s = 0; s < samples_per_channel; ++s) {
        for (int c = 0; c < channel_count; ++c) {
          // is this right?
          *out++ = (u8)buffer[c][s];
        }
      }
    } else if (bytes_per_sample == 2) {
      s16* out = (s16*)m_multiplexer.get();
      for (int s = 0; s < samples_per_channel; ++s) {
        for (int c = 0; c < channel_count; ++c) {
          *out++ = (s16)buffer[c][s];
        }
      }
    } else {
      return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    m_buffer.write(m_multiplexer.get(), total_size);
    m_position += samples_per_channel;
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
  }

}
