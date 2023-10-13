#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

typedef uint8_t     byte_t;
typedef uint16_t    half_t;
typedef uint32_t    word_t;
typedef uint64_t    dword_t;
typedef __uint128_t uint128_t;
typedef __int128_t  int128_t;
typedef uint128_t   qword_t;

typedef uint64_t reg_t;
typedef int64_t  sreg_t;

#endif
