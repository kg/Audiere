#ifndef AUDIERE_DEFAULT_IO_HPP
#define AUDIERE_DEFAULT_IO_HPP


#include "audiere.h"


extern void* ADR_CALL DefaultOpen(void* opaque, const char* filename);
extern void  ADR_CALL DefaultClose(void* file);
extern int   ADR_CALL DefaultRead(void* file, void* buffer, int size);
extern int   ADR_CALL DefaultSeek(void* file, int destination);
extern int   ADR_CALL DefaultTell(void* file);


#endif
