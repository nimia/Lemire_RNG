#define WORD_SIZE_BITS 32
#include "word_size.h"

#include "Lemire_RNG.inl"
#include "dev_urandom_utils.h"
typedef uint16_t bucket_t;
#include "test_util.inl"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_NUM_OF_TESTS_TO_RUN 1
#define PRINT_EVERY_N_WORDS (1 << 24)

int main(int argc, char *argv[])
{
  printf(
      "Welcome to the Lemire RNG 32-bit test tool! "
      "We hope your stay with us will be enjoyable.\n"
      "This tool tests that the RNG output is uniform when compiled in "
      "32-bit mode, when given UINT32_MAX (%u) as the random upper bound.\n"
      "In 32-bit mode, it is possible to exhaustively generate all outputs "
      "of the RNG and verify they are uniform.\n"
      "It then also runs argv[1] (or by default, %d) similar tests, "
      "using random 32-bit words as the upper bound.\n",
      WORD_MAX_VALUE, DEFAULT_NUM_OF_TESTS_TO_RUN);
  Lemire_RNG_test_util__test_uniformity(WORD_MAX_VALUE, PRINT_EVERY_N_WORDS);
  Lemire_RNG_test_util__test_intervals(WORD_MAX_VALUE, PRINT_EVERY_N_WORDS);

  int num_of_tests_to_run = DEFAULT_NUM_OF_TESTS_TO_RUN;
  if (argc >= 2) {
    num_of_tests_to_run = atoi(argv[1]);
  }
  for (int i = 0; i < num_of_tests_to_run; i++) {
    word_t upper_bound = dev_urandom_utils__get_random_word();
    printf("Testing with upper bound %u\n", upper_bound);
    Lemire_RNG_test_util__test_uniformity(upper_bound, PRINT_EVERY_N_WORDS);
    Lemire_RNG_test_util__test_intervals(upper_bound, PRINT_EVERY_N_WORDS);
  }
  printf(
      "Done testing, output is completely uniform on %d moduli in 32-bit "
      "mode.\n",
      num_of_tests_to_run + 1);
  return 0;
}
