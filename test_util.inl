#ifndef TEST_UTIL_INL
#define TEST_UTIL_INL

#include "Lemire_RNG.inl"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static inline void Lemire_RNG_test_util__test_uniformity(
    word_t upper_bound, uint64_t print_every_n_words)
{
  bucket_t *results = (bucket_t *)malloc(upper_bound * sizeof(bucket_t));
  assert(results);
  for (uint64_t i = 0; i < upper_bound; i++) {
    results[i] = 0;
  }

  word_t random_word_input = 0;
  do {
    word_t res;
    enum Lemire_RNG__AcceptOrReject should_accept =
        Lemire_RNG__get_random_int_or_reject(upper_bound, random_word_input,
                                             &res);
    if (should_accept == ACCEPT) {
      results[res]++;
      assert(results[res]);  // Check for overflow.
    }
    if (print_every_n_words && random_word_input % print_every_n_words == 0) {
      printf("%d%% done.\n",
             (int)((float)random_word_input / WORD_MAX_VALUE * 100));
    }
    random_word_input++;
  } while (random_word_input != 0);

  for (uint64_t i = 0; i < upper_bound; i++) {
    assert(results[i] == results[0]);
  }
  free(results);
}

static inline double_word_t
    Lemire_RNG_test_util__get_multiples_of_random_upper_bound_above_threshold(
        word_t random_word_input, word_t upper_bound)
{
  struct Lemire_RNG__Interval interval;
  // clang-tidy complains we don't read "multiple" variable, but we don't
  // need it here, and I prefer to make explicit what the result is.
  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  __int128 multiple __attribute__((unused)) =
      Lemire_RNG__multiply_and_map_to_interval(random_word_input, upper_bound,
                                               &interval);
  Lemire_RNG__set_interval_threshold(&interval, upper_bound);
  return interval.multiples_of_random_upper_bound_above_threshold;
}

static inline void Lemire_RNG_test_util__test_intervals(
    word_t upper_bound, uint64_t print_every_n_words)
{
  word_t word_input_zero = 0;

  // This value should be identical for all intervals, for all words:
  double_word_t multiples_of_random_upper_bound_above_threshold =
      Lemire_RNG_test_util__get_multiples_of_random_upper_bound_above_threshold(
          word_input_zero, upper_bound);

  word_t random_word_input = 0;
  do {
    assert(
        multiples_of_random_upper_bound_above_threshold ==
        Lemire_RNG_test_util__get_multiples_of_random_upper_bound_above_threshold(
            random_word_input, upper_bound));

    if (print_every_n_words && random_word_input % print_every_n_words == 0) {
      printf("%d%% done.\n",
             (int)((float)random_word_input / WORD_MAX_VALUE * 100));
    }
    random_word_input++;
  } while (random_word_input != 0);
}

#endif
