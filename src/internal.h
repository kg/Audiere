#ifndef AUDIERE_INTERNAL_HPP
#define AUDIERE_INTERNAL_HPP


#include "audiere.h"


#define ADR_EXPORT(ret, name)  extern "C" ret ADR_CALL name


namespace audiere {

  template<typename Interface>
  class RefCountedImplementation : public DLLImplementation<Interface> {
  public:
    RefCountedImplementation() {
      m_ref_count = 1;
    }

    void addRef() {
      ++m_ref_count;
    }

    void destroy() {
      if (--m_ref_count <= 0) {
        delete this;
      }
    }

  private:
    int m_ref_count;
  };

}


#endif
