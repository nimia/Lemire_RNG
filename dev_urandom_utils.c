#include "dev_urandom_utils.h"

#include <assert.h>
#include <stdio.h>

word_t dev_urandom_utils__get_random_word()
{
  FILE *f = fopen("/dev/urandom", "rb");
  char char_res[WORD_SIZE_BYTES];
  size_t num_of_items_read = fread(char_res, WORD_SIZE_BYTES, 1, f);
  assert(num_of_items_read == 1);
  fclose(f);
  word_t res = *(word_t *)char_res;
  return res;
}
