#ifndef CONTEXT_HPP
#define CONTEXT_HPP


#include <string>
#include "audiere.h"
#include "input.hpp"
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
  static Context* Create(
    const char* output_device,
    const char* parameters,
    IFileSystem* file_system);

private:
  Context();
  ~Context();
  bool Initialize(
    const char* output_device,
    const char* parameters,
    IFileSystem* file_system);

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

  IFileSystem* m_file_system;
  IOutputContext* m_output_context;

  friend Stream;
};


#endif
