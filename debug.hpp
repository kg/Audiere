#ifndef DEBUG_HPP
#define DEBUG_HPP


#include <stdio.h>
#include <string>


#if defined(_DEBUG) || defined(DEBUG)

  class Log {
  public:
    static void Write(const char* str);
    static void IncrementIndent() { ++indent_count; }
    static void DecrementIndent() { --indent_count; }

  private:
    static void EnsureOpen();
    static void Close();

  private:
    static FILE* handle;
    static int indent_count;
  };


  class Guard {
  public:
    Guard(const char* label)
    : m_label(label) {
      Write("+");
      Log::IncrementIndent();
    }

    ~Guard() {
      Log::DecrementIndent();
      Write("-");
    }

    void Write(const char* prefix) {
      Log::Write((prefix + m_label).c_str());
    }

  private:
    std::string m_label;
  };


  #define ADR_GUARD(label) Guard guard_obj__(label)
  #define ADR_LOG(label)   (Log::Write(label))
  #define ADR_IF_DEBUG     if (true)

  #ifdef _MSC_VER
    #define ADR_ASSERT(condition, label) if (!(condition)) { __asm int 3 }
  #else  // assume x86 gcc
    #define ADR_ASSERT(condition, label) if (!(condition)) { asm("int 3"); }
  #endif

#else

  #define ADR_GUARD(label) 
  #define ADR_LOG(label)
  #define ADR_IF_DEBUG     if (false)
  #define ADR_ASSERT(condition, label)

#endif


#endif
