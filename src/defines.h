#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i32 b32;
typedef float f32;
typedef double f64;

/* platform detection */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX
#if defined(__ANDROID__)
#define PLATFORM_ANDROID
#error "Android is not supported!"
#endif
#elif defined(__unix__)
#define PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
#define PLATFORM_POSIX
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
#define PLATFORM_IOS_SIMULATOR
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE
#define PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC
#define PLATFORM_MAC
#else
#error "Unknown Apple platform!"
#endif
#else
#error "Unknown platform!"
#endif

#endif // DEFINES_H