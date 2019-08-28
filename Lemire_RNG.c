#include "Lemire_RNG.h"

#include "Lemire_RNG.inl"

word_t Lemire_RNG__get_random_int(word_t upper_bound,
                                  word_t (*get_random_word)(),
                                  void *ctx_get_random_word)
{
  assert(upper_bound > 0);
  while (true) {
    word_t res;
    enum Lemire_RNG__AcceptOrReject should_accept =
        Lemire_RNG__get_random_int_or_reject(
            upper_bound, (*get_random_word)(ctx_get_random_word), &res);
    if (should_accept == ACCEPT) {
      assert(res < upper_bound);
      return res;
    }
  }
}
