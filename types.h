
#ifndef TYPES_H___
#define TYPES_H___

#ifdef ALPHA
typedef unsigned long QWORD;
typedef unsigned int DWORD;
#endif

#if defined X86 && defined WINDOWS
typedef __int64 QWORD;
#endif

#if defined X86 && !defined WINDOWS
typedef unsigned long long int QWORD;
typedef unsigned int DWORD;
typedef long LONG;
#endif

typedef unsigned short WORD;

typedef bool bit;
typedef unsigned char BYTE;

#endif

