#ifndef WORD_SIZE_H
#define WORD_SIZE_H

#include <stdint.h>

// clang-format off
#if WORD_SIZE_BITS == 64
  #define WORD_SIZE_BYTES 8
  typedef uint64_t word_t;
  typedef __uint128_t double_word_t;
  #define WORD_MAX_VALUE UINT64_MAX
#elif WORD_SIZE_BITS == 32
  #define WORD_SIZE_BYTES 4
  typedef uint32_t word_t;
  typedef uint64_t double_word_t;
  #define WORD_MAX_VALUE UINT32_MAX
#elif WORD_SIZE_BITS == 16
  #define WORD_SIZE_BYTES 2
  typedef uint16_t word_t;
  typedef uint32_t double_word_t;
  #define WORD_MAX_VALUE UINT16_MAX
#else
  #error "WORD_SIZE_BITS should be either 16, 32 or 64."
#endif
// clang-format on

#endif
