#ifndef FILE_HPP
#define FILE_HPP


#include "audiere.h"


extern ADR_FILE ADR_CALL DefaultFileOpen(void* opaque, const char* filename);
extern void     ADR_CALL DefaultFileClose(ADR_FILE file);
extern int      ADR_CALL DefaultFileRead(ADR_FILE file, void* buffer, int size);
extern int      ADR_CALL DefaultFileSeek(ADR_FILE file, int destination);
extern int      ADR_CALL DefaultFileTell(ADR_FILE file);


#endif
