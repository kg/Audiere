#ifndef DEBUG_HPP
#define DEBUG_HPP


#include <stdio.h>
#include <string.h>
#include <string>


#ifdef _DEBUG


namespace debug {

  class Log {
  public:
    static void Write(const char* str) {
      EnsureOpen();
      if (handle) {
        fwrite(str, 1, strlen(str), handle);
        fflush(handle);
      }

    }

  private:
    static void EnsureOpen() {
      if (!handle) {
        handle = fopen("C:/acoustique_debug.log", "w");
        atexit(Close);
      }
    }

    static void Close() {
      fclose(handle);
    }

  private:
    static FILE* handle;
  };


  class Guard {
  public:
    Guard(const char* label)
    : m_label(label) {
      write("+");
      ++s_indent_count;
    }

    ~Guard() {
      --s_indent_count;
      write("-");
    }

    static void Write(const char* label) {
      std::string s;
      for (int i = 0; i < s_indent_count; ++i) {
        s += "  ";
      }
      Log::Write((s + label + "\n").c_str());
    }

  private:
    void write(const char* prefix) {
      std::string s;
      for (int i = 0; i < s_indent_count; ++i) {
        s += "  ";
      }
      Log::Write((s + prefix + m_label + "\n").c_str());
    }

  private:
    static int s_indent_count;
    std::string m_label;
  };


  }


  #define ACQ_GUARD(label) Guard guard_obj__(label)
  #define ACQ_LOG(label)   (Guard::Write(label))
  #define ACQ_IF_DEBUG

  using namespace debug;  // I normally frown upon this, but it's nice to have them as globals


#else

  #define ACQ_GUARD(label)
  #define ACQ_LOG(label)
  #define ACQ_IF_DEBUG

#endif


#endif
