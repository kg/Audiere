#ifndef DEBUG_HPP
#define DEBUG_HPP


#include <stdio.h>
#include <string>


#ifdef _DEBUG

class Log {
public:
  static void Write(const char* str);

private:
  static void EnsureOpen();
  static void Close();

private:
  static FILE* handle;
};


class Guard {
public:
  Guard(const char* label)
  : m_label(label) {
    Write("+");
    ++s_indent_count;
  }

  ~Guard() {
    --s_indent_count;
    Write("-");
  }

  static void Write(const char* prefix);

private:
  static int s_indent_count;
  std::string m_label;
};


#define ADR_GUARD(label) Guard guard_obj__(label)
#define ADR_LOG(label)   (Guard::Write(label))
#define ADR_IF_DEBUG
#define ADR_ASSERT(condition, label)

#else

#define ADR_GUARD(label) 
#define ADR_LOG(label)
#define ADR_IF_DEBUG
#define ADR_ASSERT(condition, label)

#endif



#endif
