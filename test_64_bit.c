#define WORD_SIZE_BITS 64
#include "Lemire_RNG.h"
#include "dev_urandom_utils.h"  // for the random seed

#include <assert.h>
#include <math.h>  // sqrt()
#include <stdio.h>
#include <stdlib.h>     // rand()
#include <sys/param.h>  // MAX()
#include <time.h>

// This value should be "large", at least one million, but making it much larger
// slows down the test. This is a good sweetspot, I think.
#define NUM_OF_SAMPLES_PER_TEST ((word_t)(1 << 24))
#define PRINT_EVERY_N_WORDS (1000000)

// TODO: This should be a truly random word, from a deterministic RNG with a
// random seed, because the Lemire RNG calls this.
// I wanted to find an AES-CTR implementation or similar,
// But I don't know where to find one with an Apache license.
word_t get_random_word(__attribute__((unused)) void *dummy)
{
  return (word_t)(((double)rand()) / RAND_MAX * WORD_MAX_VALUE);
}

void test_num_samples_below_cutoff(word_t random_upper_bound,
                                   word_t cutoff_value, word_t random_seed)
{
  assert(cutoff_value < random_upper_bound);
  uint64_t num_samples_below_cutoff = 0;
  printf("Testing using %lu samples\n", NUM_OF_SAMPLES_PER_TEST);

  // We generate n samples, then check if they are below the cutoff point.
  // Each check is a random Bernoulli variable, with the following mean:
  double bernoulli_variable_mean = (double)cutoff_value / random_upper_bound;
  for (uint64_t i = 0; i < NUM_OF_SAMPLES_PER_TEST; i++) {
    word_t sample =
        Lemire_RNG__get_random_int(random_upper_bound, &get_random_word, NULL);
    if (sample < cutoff_value) {
      num_samples_below_cutoff++;
    }

    if (i % PRINT_EVERY_N_WORDS == 0) {
      printf("%d%% done.\n", (int)((float)i / NUM_OF_SAMPLES_PER_TEST * 100));
    }
  }

  // Central Limit Theorem:
  // The following value is normally distributed with mean 0 and the same
  // variance as the Bernoulli variables.
  double sqrt_n = sqrt(NUM_OF_SAMPLES_PER_TEST);
  double value_from_normal_distribution =
      (num_samples_below_cutoff / sqrt_n) - (bernoulli_variable_mean * sqrt_n);

  double bernoulli_variable_variance =
      bernoulli_variable_mean * (1 - bernoulli_variable_mean);
  double bernoulli_variable_stdev = sqrt(bernoulli_variable_variance);

  int num_of_sigmas_to_fail_test = 8;
  // If we have a value from a normal distribution that is 8 sigmas or more
  // far from the mean, the probability this happened is one in 8*10^14.
  // Even if one thousand different CI systems run this test every day for ten
  // years, the probability it will fail is one in 8*10^14 / 10^3 / 3650 = 10^8.
  // So this test should NEVER fail. Even though the test is probabilistic, if
  // it failed there is almost certainly an issue with the RNG.
  double deviation_from_mean_in_sigmas =
      fabs(value_from_normal_distribution) / bernoulli_variable_stdev;
  printf(
      "FYI the number from the normal distribution is %f sigmas away from "
      "the mean (the smaller, the more reassuring).\n",
      deviation_from_mean_in_sigmas);
  if (deviation_from_mean_in_sigmas > num_of_sigmas_to_fail_test) {
    printf(
        "Error in statistical test - "
        "there is almost certainly an issue with the RNG.\n"
        "This test was run with random seed %lu\n."
        "You can reproduce this run by passing the seed as the first "
        "argument.\n",
        random_seed);
    exit(1);
  } else {
    printf("Looks OK, test considered successful.\n");
  }
}

int main(int argc, char *argv[])
{
  word_t random_seed;
  if (argc > 2) {
    random_seed = atoi(argv[1]);
  } else {
    random_seed = dev_urandom_utils__get_random_word();
  }
  srand(random_seed);

  printf(
      "Welcome to the Lemire RNG 64-bit test tool! "
      "We hope your stay with us will be enjoyable.\n"
      "The tool generates %lu words from the RNG, "
      "selects a random cutoff value, and tests that the expected proportion "
      "of words from the RNG are below the cutoff value.\n"
      "It does so twice, once with WORD_MAX_VALUE (%lu) as the random upper "
      "bound, and once with a randomly-selected upper bound.\n",
      NUM_OF_SAMPLES_PER_TEST, WORD_MAX_VALUE);
  printf("FYI, your random seed for today is %lu.\n", random_seed);

  test_num_samples_below_cutoff(WORD_MAX_VALUE, get_random_word(NULL),
                                random_seed);

  word_t random_word1 = get_random_word(NULL);
  word_t random_word2 = get_random_word(NULL);
  word_t random_upper_bound = MAX(random_word1, random_word2);
  word_t cutoff_value = MIN(random_word1, random_word2);
  test_num_samples_below_cutoff(random_upper_bound, cutoff_value, random_seed);
}
