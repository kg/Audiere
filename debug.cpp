#ifdef _DEBUG

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
    handle = fopen("C:/audiere_debug.log", "w");
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
