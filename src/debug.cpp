#if defined(_DEBUG) || defined(DEBUG)

#include "debug.hpp"


FILE* Log::handle;
int Log::indent_count;


////////////////////////////////////////////////////////////////////////////////

void
Log::Write(const char* str)
{
  EnsureOpen();
  if (handle) {
    std::string s;
    for (int i = 0; i < indent_count; ++i) {
      s += "  ";
    }
    s += str;
    s += "\n";
    fwrite(s.c_str(), 1, s.length(), handle);
    fflush(handle);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
Log::EnsureOpen()
{
  if (!handle) {

    #ifdef WIN32
      handle = fopen("C:/audiere_debug.log", "w");
    #else
      std::string home(getenv("HOME"));
      handle = fopen((home + "/audiere_debug.log").c_str(), "w");
    #endif

    atexit(Close);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
Log::Close()
{
  fclose(handle);
}

////////////////////////////////////////////////////////////////////////////////


#endif
