#pragma once
#if defined(_WIN32)
#include "windows.h"
#endif
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint32_t  u32;
typedef int32_t   i32;
typedef uint64_t  u64;
typedef int64_t   i64;
typedef float     f32;
typedef double    f64;
typedef int32_t   b32;
typedef char      b8;//NOTE(ilias): check to be sure!!

#define equalf(a, b, epsilon) (fabs(b - a) <= epsilon)
#define maximum(a, b) ((a) > (b) ? (a) : (b))
#define minimum(a, b) ((a) < (b) ? (a) : (b))
#define clamp(x, a, b)  (maximum(a, minimum(x, b)))
#define array_count(a) (sizeof(a) / sizeof((a)[0]))


#define PI 3.1415926535897f

static b32
char_is_alpha(i32 c)
{
    return ((c >='A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}
static b32 char_is_digit(i32 c)
{
    return c >= '0'&& c <= '9';
}

static i32 char_to_lower(i32 c)
{
    if (c >= 'A' && c <= 'Z')
    {
        c += 32;
    }
    return c;
}

static void
seed_random_()
{
    srand((u32)time(0));
}


/*NOTE(ilias): BEWARE this is C++ 
 * Close your eyes or suffer! 
 * TODO(ilias): remove this abomination*/
#include <string>
#include <fstream>









