#ifndef FILE_HPP
#define FILE_HPP


#include "audiere.h"


extern void* ADR_CALL DefaultFileOpen(void* opaque, const char* filename);
extern void  ADR_CALL DefaultFileClose(void* file);
extern int   ADR_CALL DefaultFileRead(void* file, void* buffer, int size);
extern int   ADR_CALL DefaultFileSeek(void* file, int destination);
extern int   ADR_CALL DefaultFileTell(void* file);


#endif
