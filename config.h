#ifndef AUDIERE_CONFIG_H
#define AUDIERE_CONFIG_H

#  ifdef _MSC_VER  // VC++

typedef unsigned __int8  adr_u8;
typedef signed   __int8  adr_s8;
typedef unsigned __int16 adr_u16;
typedef signed   __int16 adr_s16;
typedef unsigned __int32 adr_u32;
typedef signed   __int32 adr_s32;
typedef unsigned __int64 adr_u64;
typedef signed   __int64 adr_s64;

#  else            // assume gcc i386 linux ;)

typedef unsigned char      adr_u8;
typedef signed   char      adr_s8;
typedef unsigned short     adr_u16;
typedef signed   short     adr_s16;
typedef unsigned long      adr_u32;
typedef signed   long      adr_s32;
typedef unsigned long long adr_u64;
typedef signed   long long adr_s64;

#  endif

#endif
