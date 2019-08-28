#ifndef LEMIRE_RNG_INL
#define LEMIRE_RNG_INL

#include "word_size.h"

#include <assert.h>
#include <stdbool.h>

struct Lemire_RNG__Interval {
  double_word_t interval_lower_bound;
  double_word_t interval_upper_bound;
  double_word_t reject_multiples_below_threshold;
  double_word_t multiples_of_random_upper_bound_above_threshold;
};

// We multiply x by random_upper_bound, and the upper word of the result
// determines which interval we landed in.
static inline double_word_t Lemire_RNG__multiply_and_map_to_interval(
    word_t x, word_t random_upper_bound, struct Lemire_RNG__Interval *interval)
{
  double_word_t multiple = (double_word_t)x * (double_word_t)random_upper_bound;
  interval->interval_lower_bound = (multiple >> WORD_SIZE_BITS)
                                   << WORD_SIZE_BITS;
  interval->interval_upper_bound = ((multiple >> WORD_SIZE_BITS) + 1)
                                   << WORD_SIZE_BITS;
  assert(interval->interval_lower_bound <= multiple &&
         multiple < interval->interval_upper_bound);
  return multiple;
}

static inline bool Lemire_RNG__is_greater_equal_to_threshold(
    double_word_t multiple, const struct Lemire_RNG__Interval *interval,
    word_t random_upper_bound)
{
  if (interval->interval_lower_bound + random_upper_bound <= multiple) {
    // Optimization to accept samples without doing modulo computations.
    assert(interval->reject_multiples_below_threshold <=
           interval->interval_lower_bound + random_upper_bound);
    assert(interval->reject_multiples_below_threshold <= multiple);
    return true;
  }
  return (interval->reject_multiples_below_threshold <= multiple);
}

static inline bool Lemire_RNG__should_accept(
    double_word_t multiple, const struct Lemire_RNG__Interval *interval,
    word_t random_upper_bound)
{
  return Lemire_RNG__is_greater_equal_to_threshold(multiple, interval,
                                                   random_upper_bound);
}

enum Lemire_RNG__AcceptOrReject { ACCEPT, REJECT };

// The number of multiples of random_upper_bound above the threshold is the same
// for all intervals. This is why the RNG is unbiased.
static inline void Lemire_RNG__set_interval_threshold(
    struct Lemire_RNG__Interval *interval, word_t random_upper_bound)
{
  interval->reject_multiples_below_threshold =
      interval->interval_lower_bound +
      (((double_word_t)1 << WORD_SIZE_BITS) % random_upper_bound);
  assert((interval->interval_upper_bound -
          interval->reject_multiples_below_threshold) %
             random_upper_bound ==
         0);
  // This is the same for all intervals, which is why the output is unbiased.
  interval->multiples_of_random_upper_bound_above_threshold =
      (interval->interval_upper_bound -
       interval->reject_multiples_below_threshold) /
      random_upper_bound;
}

__attribute__((
    warn_unused_result)) static inline enum Lemire_RNG__AcceptOrReject
    Lemire_RNG__get_random_int_or_reject(word_t upper_bound, word_t random_word,
                                         word_t *res_random_int)
{
  *res_random_int = (word_t)(-1);
  struct Lemire_RNG__Interval interval;
  // multiple is between 0 and upper_bound * 2^WORD_SIZE_BITS
  __int128 multiple = Lemire_RNG__multiply_and_map_to_interval(
      random_word, upper_bound, &interval);
  Lemire_RNG__set_interval_threshold(&interval, upper_bound);
  if (!Lemire_RNG__should_accept(multiple, &interval, upper_bound)) {
    return REJECT;
  }
  // the result is between 0 and upper_bound.
  *res_random_int = multiple >> WORD_SIZE_BITS;
  return ACCEPT;
}

#endif
