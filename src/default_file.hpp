#ifndef DEFAULT_FILE_HPP
#define DEFAULT_FILE_HPP


extern ADR_FILE ADR_CALL DefaultFileOpen(
  void* opaque,
  const char* filename);

extern void ADR_CALL DefaultFileClose(
  ADR_FILE file);

extern int ADR_CALL DefaultFileRead(
  ADR_FILE file,
  void* buffer,
  int size);

extern int ADR_CALL DefaultFileSeek(
  ADR_FILE file,
  int destination,
  ADR_SEEK_TYPE origin);

extern int ADR_CALL DefaultFileTell(
  ADR_FILE file);


#endif
