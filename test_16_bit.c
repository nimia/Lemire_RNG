#define WORD_SIZE_BITS 16
#include "word_size.h"

#include "Lemire_RNG.inl"
typedef uint32_t bucket_t;
#include "test_util.inl"

#include <stdio.h>
#include <stdlib.h>

#define PRINT_EVERY_N_TESTS 1000

int main(int argc, char *argv[])
{
  printf(
      "Welcome to the Lemire RNG 16-bit test tool! "
      "We hope your stay with us will be enjoyable.\n"
      "This tool tests that the RNG output is uniform when compiled in "
      "16-bit mode.\n"
      "The tool iterates over all possible 2^16 upper bounds, and for each "
      "upper bound, exhaustively generates all outputs of the RNG.\n"
      "This allows verifying that the RNG is completely uniform, in 16-bit "
      "mode.\n");

  word_t upper_bound = 1;
  if (argc >= 2) {
    upper_bound = atoi(argv[1]);
  }
  do {
    Lemire_RNG_test_util__test_uniformity(upper_bound, 0);
    Lemire_RNG_test_util__test_intervals(upper_bound, 0);
    if (upper_bound % PRINT_EVERY_N_TESTS == 0) {
      printf("Tested up to and including %d, all uniform so far.\n",
             upper_bound);
    }
    upper_bound++;
  } while (upper_bound != 0);
  printf("Done testing, output is completely uniform in 16 bit mode.\n");
  return 0;
}
