#ifndef BASIC_SOURCE_H
#define BASIC_SOURCE_H


#include <vector>
#include <string>
#include "audiere.h"
#include "utility.h"
#include "debug.h"


namespace audiere {

  struct Tag {
    Tag(const std::string& k, const std::string& v, const std::string& t) {
      key = k;
      value = v;
      type = t;
    }

    std::string key;
    std::string value;
    std::string type;
  };

  /**
   * Template hack to automatically implement asMultichannel depending on
   * whether a source is multichannel or not.
   */

  template <typename T>
  inline MultichannelSampleSource * SourceToMultichannel (T * source) {
      return 0;
  }

  template <>
  inline MultichannelSampleSource * SourceToMultichannel<MultichannelSampleSource> (MultichannelSampleSource * source) {
      return source;
  }

  /**
   * Basic implementation of a sample source including things such as
   * repeat.  BasicSource also defines the required methods for unseekable
   * sources.  Override them if you can seek.
   */
  template <typename T>
  class TBasicSource : public RefImplementation<T> {
  public:
    TBasicSource() {
      m_repeat = false;
    }

    /**
     * Manages repeating within read().  Implement doRead() in
     * implementation classes.
     */
    int ADR_CALL read(int frame_count, void* buffer) {
        if (m_repeat) {
          const int frame_size = GetFrameSize(this);

          // the main read loop:
          u8* out = (u8*)buffer;
          int frames_left = frame_count;
          while (frames_left > 0) {

            // read some frames.  if we can't read anything, reset the stream
            // and try again.
            int frames_read = doRead(frames_left, out);
            if (frames_read == 0) {
              reset();
              frames_read = doRead(frames_left, out);

              // if we still can't read anything, we're done
              if (frames_read == 0) {
                ADR_LOG("Can't read any samples even after reset");
                break;
              }
            }

            frames_left -= frames_read;
            out += frames_read * frame_size;
          }

          return frame_count - frames_left;

        } else {

          return doRead(frame_count, buffer);

        }
    }

    bool ADR_CALL isSeekable()                  { return false; }
    int  ADR_CALL getLength()                   { return 0;     }
    void ADR_CALL setPosition(int /*position*/) {               }
    int  ADR_CALL getPosition()                 { return 0;     }

    bool ADR_CALL getRepeat()                   { return m_repeat; }
    void ADR_CALL setRepeat(bool repeat)        { m_repeat = repeat; }

    int ADR_CALL getTagCount()              { return int(m_tags.size()); }
    const char* ADR_CALL getTagKey(int i)   { return m_tags[i].key.c_str(); }
    const char* ADR_CALL getTagValue(int i) { return m_tags[i].value.c_str(); }
    const char* ADR_CALL getTagType(int i)  { return m_tags[i].type.c_str(); }
    const char* ADR_CALL getDecoder()       { return m_decoder_text.c_str(); }
    MultichannelSampleSource * ADR_CALL asMultichannel() { return SourceToMultichannel<T>(this); }

    /// Implement this method in subclasses.
    virtual int doRead(int frame_count, void* buffer) = 0;

  protected:
    void addTag(const Tag& t) {
      m_tags.push_back(t);
    }

    void addTag(const std::string& k, const std::string& v, const std::string& t) {
      addTag(Tag(k, v, t));
    }

    std::string m_decoder_text;

  private:
    bool m_repeat;
    std::vector<Tag> m_tags;
  };

  typedef TBasicSource<SampleSource> BasicSource;
  typedef TBasicSource<MultichannelSampleSource> BasicMultichannelSource;

}


#endif
