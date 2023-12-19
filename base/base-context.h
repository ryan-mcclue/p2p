// SPDX-License-Identifier: zlib-acknowledgement
#pragma once


// NOTE(Ryan): GCC
#if defined(__GNUC__)
  #define COMPILER_GCC 1

  // NOTE(Ryan): Version
  #if __GNUC__ < 10
    #warning GCC 10+ required for builtin static analysis
  #endif

  // NOTE(Ryan): Architecture
  #if defined(__x86_64__)
    #define ARCH_X86_64 1
  #elif defined(__arm__) 
    #if defined(__aarch64__)
      #define ARCH_ARM64 1
    #else
      #define ARCH_ARM32 1
      #if defined(__ARM_ARCH_7A__)
        #define ARCH_ARM7A 1
      #endif
    #endif
  #elif defined(__XTENSA__)
    #define ARCH_XTENSA 1
  #else
    #error Arch not supported
  #endif

  // NOTE(Ryan): Platform
  #if defined(__gnu_linux__)
    #define PLATFORM_LINUX 1
  #elif defined(ARCH_ARM)
    #define PLATFORM_ARM_EABI 1
    // TODO(Ryan): Are there cortex specific defines?
  #elif defined(ESP_PLATFORM) && (ESP_PLATFORM == 1)
    #define PLATFORM_ESP32 1
  #else
    #error Platform not supported
  #endif

  // NOTE(Ryan): Language
  #if defined(__cplusplus)
    #define LANG_CPP 1
    #if __cplusplus <= 199711L
      #define CPP_VERSION 98
    #elif __cplusplus <= 201103L
      #define CPP_VERSION 11
    #else
      #define CPP_VERSION 0
    #endif
  #else
    #define LANG_C 1
    #if __STDC_VERSION__ <= 199901L
      #define C_VERSION 99
    #elif __STDC_VERSION__ <= 201112L
      #define C_VERSION 11
    #else
      #define C_VERSION 0
    #endif
  #endif
  
  #if LANG_CPP
    // NOTE(Ryan): Avoid confusing auto-indenter
    #define EXPORT_BEGIN extern "C" {
    #define EXPORT_END }
    #define EXPORT extern "C"
    #define ZERO_STRUCT {}
    #define RESTRICT __restrict__
  #else
    #define EXPORT_BEGIN
    #define EXPORT_END
    #define EXPORT
    #define RESTRICT restrict
    #define ZERO_STRUCT {0}
  #endif

  // NOTE(Ryan): Sanitiser
  #if defined(__SANITIZE_ADDRESS__)
    #define NO_ASAN __attribute__((__no_sanitize_address__))
    #include <sanitizer/lsan_interface.h>
    // NOTE(Ryan): Prevent unreachables resulting from sanitiser being run after main
    #define LSAN_RUN() __lsan_do_leak_check(); __lsan_disable()
  #else
    #define NO_ASAN
    #define LSAN_RUN()
  #endif

  // NOTE(Ryan): Extensions
  #define CASE_FALLTHROUGH __attribute__((fallthrough))
  #define NEVER_INLINE   __attribute__((noinline))
  #define USED __attribute__((used,noinline))
  #define ALWAYS_INLINE __attribute__((optimize("inline-functions"),always_inline))
  #define PACKED __attribute__((packed))
  #define NORETURN __attribute__((noreturn))
  #define UNUSED __attribute__((unused))
  #define UNREACHABLE() __builtin_unreachable()
  #define WEAK __attribute__((weak))
  #define LIKELY(x)   __builtin_expect(!!(x), 1) 
  #define UNLIKELY(x) __builtin_expect(!!(x), 0)
  #define ISO_EXTENSION __extension__
  // NOTE(Ryan): Used to ensure .text/program-memory aligned on ARM
  #define PROGMEM const __attribute__ ((aligned(4)))
  // TODO(Ryan): Could place in specific section and watch for it in gdb?
  // #define READ_ONLY __attribute__((section(".roglob"))) 
  // watch *(int *)0xaddress_of_variable_in_roglob_section

  // TODO(Ryan): tail cail compiler macro?
  // https://blog.reverberate.org/2021/04/21/musttail-efficient-interpreters.html
  
  // TODO(Ryan): Synchronisation atomics, e.g:
  // #define AtomicAdd64(ptr, v) _InterlockedExchangeAdd64((ptr), (v))
  // #define MEMORY_BARRIER()
  
  #if defined(PLATFORM_LINUX)
    #define THREAD_LOCAL __thread
  #else
    #define THREAD_LOCAL 
  #endif

  // NOTE(Ryan): 
  #define PUSH_OPTIMISATION_MODE() \
    _Pragma("GCC push_options") \
    _Pragma("GCC optimize (\"O3\")")

  #define POP_OPTIMISATION_MODE() \
    _Pragma("GCC pop_options")

  #define IGNORE_WARNING_PADDED() \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpadded\"")

  #define IGNORE_WARNING_PEDANTIC() \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"")

  #define IGNORE_WARNING_POP() \
    _Pragma("GCC diagnostic pop")
#else
  #error Compiler not supported
#endif

#include <stdio.h>
static void
print_context(void)
{
  printf("Base Context Detected:\n");

#if defined(ARCH_X86_64)
  printf(" - Arch: x86_64\n");
#elif defined(ARCH_ARM64)
  printf(" - Arch: arm64\n");
#elif defined(ARCH_ARM32)
  printf(" - Arch: arm32\n");
#elif defined(ARCH_XTENSA)
  printf(" - Arch: xtensa\n");
#endif

#if defined(PLATFORM_LINUX)
  printf(" - Platform: linux\n");
#elif defined(PLATFORM_ARM_EABI)
  printf(" - Platform: arm-eabi\n");
#elif defined(PLATFORM_ESP32)
  printf(" - Platform: esp32\n");
#endif

#if defined(COMPILER_GCC)
  printf(" - Compiler: gcc (%d.%d.%d)\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
 #if defined(IDF_VER)
    printf("   ESP-IDF (%d.%d.%d)\n", IDF_VER_MAJOR, IDF_VER_MINOR, IDF_VER_PATCH);
 #endif
#endif

#if defined(LANG_CPP)
  printf(" - C++ version: %d\n", CPP_VERSION);
#elif defined(LANG_C)
  printf(" - C version: %d\n", C_VERSION);
#endif
}
