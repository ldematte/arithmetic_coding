#ifndef COMMON_H__
#define COMMON_H__

//#define NDEBUG
#define X86
//#define WINDOWS

#include "types.h"
#include <assert.h>
#include <stdio.h>

#ifdef WINDOWS

#include <windows.h>
#pragma warning (disable: 4786)
#include "G:\\Debug\\include\\BugslayerUtil.h"

#else

#define _MAX max
#define _MIN min

#ifdef NDEBUG
#define ASSERT(x) { if (!(x)) perror("Last error");  assert(x); }
#define TRACE printf
#else
#define ASSERT(x) 
#endif //NDEBUG

#endif //WINDOWS

#define MSB_MASK (0x80000000)
#define UPPER_LIMIT (~0x0)

const DWORD g_dwMagic = 0x68746542;

#define MSB(x)  (x >> ((sizeof(x) * 8) - 1))
#define M2SB(x) ((x >> ((sizeof(x) * 8) - 2)) & 0x1)
#define LSB(x)  (x & 0x0001)

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#endif

