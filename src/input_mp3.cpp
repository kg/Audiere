/*
  THIS IS WRITTEN FOR CHAD AUSTIN FOR AUDIERE
  By Jacky Chong

  In short, this is some wierd shit I wrote up because Chad
  didn't want to touch this. Also, I have no idea what I've
  done as well to get it work as well.

  I've since done a bunch of work on it.  :)  -Chad
*/

#include <string.h>
#include "input_mp3.h"
#include "utility.h"
#include "debug.h"


// The number of MP3 frame that are processed at a time.  If this value
// is smaller, the decoder should take less memory.  However, it may
// skip on corrupt MP3s.
static const int FRAME_COUNT = 10;


namespace audiere {


  class MyLoader : public Soundinputstream {
  public:
    MyLoader(FilePtr file) {
      m_file = file;
      m_eof = false;
    }

    int getbytedirect() {
      u8 b;
      if (m_file->read(&b, 1) == 1) {
        return b;
      } else {
        seterrorcode(SOUND_ERROR_FILEREADFAIL);
        m_eof = true;
        return -1;
      }
    }

    bool _readbuffer(char* buffer, int size) {
      if (m_file->read(buffer, size) == size) {
        return true;
      } else {
        seterrorcode(SOUND_ERROR_FILEREADFAIL);
        m_eof = true;
        return false;
      }
    }

    bool eof() {
      return m_eof;
    }

    int getblock(char* buffer, int size) {
      int read = m_file->read(buffer, size);
      if (read != size) {
        m_eof = true;
      }
      return read;
    }

    int getsize() {
      int pos = m_file->tell();
      m_file->seek(0, File::END);
      int size = m_file->tell();
      m_file->seek(pos, File::BEGIN);
      return size;
    }

    void setposition(int pos) {
      m_file->seek(pos, File::BEGIN);
      m_eof = false;
    }

    int getposition() {
      return m_file->tell();
    }

  private:
    FilePtr m_file;
    bool m_eof;
  };


  MP3InputStream::MP3InputStream() {
    m_file = 0;

    m_channel_count = 2;
    m_sample_rate = 44100;
    m_sample_format = SF_S16;

    m_decoder = 0;
    m_loader = 0;
  }

  
  MP3InputStream::~MP3InputStream() {
    delete m_decoder;
    delete m_loader;
  }


  bool
  MP3InputStream::initialize(FilePtr file) {
    m_file = file;
    readID3v1Tags();
    readID3v2Tags();
    m_file->seek(0, File::BEGIN);

    m_loader = new MyLoader(file);
    m_decoder = new Mpegtoraw(m_loader, this);

    m_decoder->initialize();

    // this should call setsoundtype with the format of the stream
    if (!m_decoder->run(FRAME_COUNT)) {
      return false;
    }

    return true;
  }


  void
  MP3InputStream::getFormat(
    int& channel_count, 
    int& sample_rate, 
    SampleFormat& sample_format) 
  {
    channel_count = m_channel_count;
    sample_rate = m_sample_rate;
    sample_format = m_sample_format;
  }

  
  int
  MP3InputStream::doRead(int frame_count, void* samples) {
    ADR_GUARD("MP3InputStream::doRead");

    const int frame_size = m_channel_count * GetSampleSize(m_sample_format);

    int frames_read = 0;
    u8* out = (u8*)samples;

    while (frames_read < frame_count) {

      // no more samples?  ask the MP3 for more
      if (m_buffer.getSize() < frame_size) {
        if (!m_decoder->run(FRAME_COUNT)) {
          // done decoding?
          return frames_read;
        }

        // if the buffer is still empty, we are done
        if (m_buffer.getSize() < frame_size) {
          return frames_read;
        }
      }

      const int frames_left = frame_count - frames_read;
      const int frames_to_read = std::min(
        frames_left,
        m_buffer.getSize() / frame_size);

      m_buffer.read(out, frames_to_read * frame_size);
      out += frames_to_read * frame_size;
      frames_read += frames_to_read;
    }

    return frames_read;
  }


  void
  MP3InputStream::reset() {
    ADR_GUARD("MP3InputStream::reset");

    m_file->seek(0, File::BEGIN);

    m_buffer.clear();
    m_channel_count = 2;
    m_sample_rate = 44100;
    m_sample_format = SF_S16;

    delete m_decoder;
    delete m_loader;

    m_loader = new MyLoader(m_file.get());
    m_decoder = new Mpegtoraw(m_loader, this);

    m_decoder->initialize();

    // this should call setsoundtype with the format of the stream
    if (!m_decoder->run(FRAME_COUNT)) {
      return;
    }
  }


  bool
  MP3InputStream::setsoundtype(int stereo, int samplesize, int speed) {
    m_channel_count = (stereo ? 2 : 1);
    m_sample_rate = speed;

    if (samplesize == 8) {
      m_sample_format = SF_U8;
    } else if (samplesize == 16) {
      m_sample_format = SF_S16;
    } else {
      return false;
    }

    return true;
  }


  bool
  MP3InputStream::putblock(void* buffer, int size) {
    m_buffer.write(buffer, size);
    return true;
  }


  const char* getGenre(u8 code) {
    const char* genres[] = {
      // From Appendix A.3 at http://www.id3.org/id3v2-00.txt and

      "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
      "Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies", "Other",
      "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno", "Industrial",
      "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
      "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
      "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House",
      "Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass",
      "Soul", "Punk", "Space", "Meditative", "Instrumental Pop",
      "Instrumental Rock", "Ethnic", "Gothic", "Darkwave",
      "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",
      "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40",
      "Christian Rap", "Pop/Funk", "Jungle", "Native American",
      "Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes",
      "Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka",
      "Retro", "Musical", "Rock & Roll", "Hard Rock", "Folk", "Folk-Rock",
      "National Folk", "Swing", "Fast Fusion", "Bebob", "Latin", "Revival",
      "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
      "Progressive Rock", "Psychedelic Rock", "Symphonic Rock",
      "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic",
      "Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata",
      "Symphony", "Booty Bass", "Primus", "Porn Groove", "Satire",
      "Slow Jam", "Club", "Tango", "Samba", "Folklore", "Ballad",
      "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet", "Punk Rock",
      "Drum Solo", "Acapella", "Euro-House", "Dance Hall",
      
      // http://lame.sourceforge.net/doc/html/id3.html

      "Goa", "Drum & Bass", "Club-House", "Hardcore", "Terror", "Indie",
      "BritPop", "Negerpunk", "Polsk Punk", "Beat", "Christian Gangsta",
      "Heavy Metal", "Black Metal", "Crossover", "Contemporary C",
      "Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime",
      "JPop", "SynthPop",
    };
    const int genre_count = sizeof(genres) / sizeof(*genres);

    return (code < genre_count ? genres[code] : "");
  }


  // Return a null-terminated std::string from the beginning of 'buffer'
  // up to 'maxlen' chars in length.
  std::string getString(u8* buffer, int maxlen) {
    char* begin = reinterpret_cast<char*>(buffer);
    int end = 0;
    for (; end < maxlen && begin[end]; ++end) {
    }
    return std::string(begin, begin + end);
  }


  void
  MP3InputStream::readID3v1Tags() {
    // Actually, this function reads both ID3v1 and ID3v1.1.

    if (!m_file->seek(-128, File::END)) {
      return;
    }

    u8 buffer[128];
    if (m_file->read(buffer, 128) != 128) {
      return;
    }

    // Verify that it's really an ID3 tag.
    if (memcmp(buffer + 0, "TAG", 3) != 0) {
      return;
    }

    std::string title   = getString(buffer + 3,  30);
    std::string artist  = getString(buffer + 33, 30);
    std::string album   = getString(buffer + 63, 30);
    std::string year    = getString(buffer + 93, 4);
    std::string comment = getString(buffer + 97, 30);
    std::string genre   = getGenre(buffer[127]);

    addTag("title",   title,   "ID3v1");
    addTag("artist",  artist,  "ID3v1");
    addTag("album",   album,   "ID3v1");
    addTag("year",    year,    "ID3v1");
    addTag("comment", comment, "ID3v1");
    addTag("genre",   genre,   "ID3v1");

    // This is the ID3v1.1 part.
    if (buffer[97 + 28] == 0 && buffer[97 + 29] != 0) {
      char track[20];
      sprintf(track, "%d", int(buffer[97 + 29]));
      addTag("track", track, "ID3v1.1");
    }
  }

  
  void
  MP3InputStream::readID3v2Tags() {
    // ID3v2 is super complicated.
  }

}
