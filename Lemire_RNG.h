#ifndef LEMIRE_RNG_H
#define LEMIRE_RNG_H

#include "word_size.h"

// Return a random word_t which is less than upper_bound.
word_t Lemire_RNG__get_random_int(word_t upper_bound,
                                  word_t (*get_random_word)(),
                                  void *ctx_get_random_word);

#endif
