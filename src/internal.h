#ifndef AUDIERE_INTERNAL_H
#define AUDIERE_INTERNAL_H


#include "audiere.h"


#define ADR_EXPORT(ret, name)  extern "C" ret ADR_CALL name


namespace audiere {

  template<typename Interface>
  class RefCountedImplementation : public DLLImplementation<Interface> {
  public:
    RefCountedImplementation() {
      m_ref_count = 1;
    }

    void ref() {
      ++m_ref_count;
    }

    void unref() {
      if (--m_ref_count <= 0) {
        delete this;
      }
    }

    void destroy() {
      unref();
    }

  private:
    int m_ref_count;
  };

}


#endif
