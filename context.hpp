#ifndef CONTEXT_HPP
#define CONTEXT_HPP


#include <string>
#include "audiere.h"
#include "output.hpp"
#include "threads.hpp"


struct ADR_CONTEXT_ATTRimp
{
  std::string    output_device;
  std::string    parameters;
  void*          opaque;
  ADR_FILE_OPEN  open;
  ADR_FILE_CLOSE close;
  ADR_FILE_READ  read;
  ADR_FILE_SEEK  seek;
  ADR_FILE_TELL  tell;
};


class Stream;


class Context : public Synchronized
{
public:
  static Context* Create(ADR_CONTEXT_ATTR attr);

private:
  Context() { }  // Initialize() is the constructor
  ~Context();
  bool Initialize(ADR_CONTEXT_ATTR attr);

public:
  void AddRef();
  void Release();

  Stream* OpenStream(const char* filename);

private:
  static void ThreadRoutine(void* opaque);
  void ThreadRoutine();

private:
  int m_ref_count;
  volatile bool m_thread_should_die;

  // file I/O
  void*          m_opaque;
  ADR_FILE_OPEN  m_open;
  ADR_FILE_CLOSE m_close;
  ADR_FILE_READ  m_read;
  ADR_FILE_SEEK  m_seek;
  ADR_FILE_TELL  m_tell;

  IOutputContext* m_output_context;

  friend Stream;
};


#endif
