#ifndef FILE_HPP
#define FILE_HPP


#include "audiere.h"


class IFile
{
public:
  virtual ~IFile() { }

  virtual int  Read(void* bytes, int byte_count) = 0;
  virtual bool Seek(int offset, ADR_SEEK_TYPE type) = 0;
  virtual int  Tell() = 0;
};


class IFileSystem
{
public:
  virtual IFile* Open(const char* filename) = 0;
};


class ADRFile;


class ADRFileSystem : public IFileSystem
{
public:
  ADRFileSystem(
    void* opaque,
    ADR_FILE_OPEN open,
    ADR_FILE_CLOSE close,
    ADR_FILE_READ read,
    ADR_FILE_SEEK seek,
    ADR_FILE_TELL tell);

  IFile* Open(const char* filename);

private:
  void*          m_opaque;
  ADR_FILE_OPEN  m_open;
  ADR_FILE_CLOSE m_close;
  ADR_FILE_READ  m_read;
  ADR_FILE_SEEK  m_seek;
  ADR_FILE_TELL  m_tell;

  friend class ADRFile;
};


class ADRFile : public IFile
{
public:
  ADRFile(ADRFileSystem* file_system, ADR_FILE file);
  ~ADRFile();

  int  Read(void* bytes, int byte_count);
  bool Seek(int offset, ADR_SEEK_TYPE type);
  int  Tell();

private:
  ADRFileSystem* m_file_system;
  ADR_FILE       m_file;

  friend class ADRFileSystem;
};


#endif
