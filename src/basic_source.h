#ifndef BASIC_SOURCE_H
#define BASIC_SOURCE_H


#include "audiere.h"


namespace audiere {

  /**
   * Basic implementation of a sample source including things such as
   * repeat.  BasicSource also defines the required methods for unseekable
   * sources.  Override them if you can seek.
   */
  class BasicSource : public RefImplementation<SampleSource> {
  public:
    BasicSource();

    /**
     * Manages repeating within read().  Implement doRead() in
     * implementation classes.
     */
    int ADR_CALL read(int frame_count, void* buffer);

    bool ADR_CALL isSeekable()                  { return false; }
    int  ADR_CALL getLength()                   { return 0;     }
    void ADR_CALL setPosition(int /*position*/) {               }
    int  ADR_CALL getPosition()                 { return 0;     }

    bool ADR_CALL getRepeat()                   { return m_repeat; }
    void ADR_CALL setRepeat(bool repeat)        { m_repeat = repeat; }

    /// Implement this method in subclasses.
    virtual int doRead(int frame_count, void* buffer) = 0;

  private:
    bool m_repeat;
  };

}


#endif
