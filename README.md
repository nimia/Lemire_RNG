# DO NOT USE THIS CODE!!!
This is a toy, not production-ready code.
It is a learning project implementing Lemire's Nearly Divisionless Random Integer Generation in C.
Random Number Generators (henceforth, RNGs) must be well-tested before used in production.
In particular, 
[some cryptographic mechanisms fail catastrophically](https://ljk.imag.fr/membres/Jean-Guillaume.Dumas/Enseignements/ProjetsCrypto/FautesECC/Nguyen.pdf) when used with biased RNGs.
So don't use this!

# A C implementation of Lemire's Nearly Divisionless RNG
The original Lemire paper can be found [here](https://arxiv.org/pdf/1805.10941.pdf), and an accompanying blogpost can be found [here](https://lemire.me/blog/2019/06/06/nearly-divisionless-random-integer-generation-on-various-systems/).

The main implementation choice is to follow the arguments from the paper as closely as possible.
The code contains several functions that follow Lemma 4.1 from the paper.
Since we're using C, the functions are declared as "static inline" and should be inlined and optimized by the compiler, so in principle this should not result in a performance slowdown (if you disable the calls to assert()).

## Testing Strategy
It is obviously non-trivial to test that an RNG is unbiased.
However, we observe that when the word size is 32 bits, it is possible to exhaustively generate all RNG outputs for a given upper bound.
This allows actually confirming that the output is uniform.

Specifically, running `make test` will:
1. Compile the code with 16-bit word size, iterate over all 2^16 possible upper bounds.

  1.1 For each upper bound iterate over all 2^16 possible inputs to the RNG, verify the RNG output is uniform for this upper bound.

2. Compile the code with 32-bit word size, choose 2 upper bounds: WORD_MAX and a value from /dev/urandom.

  2.1. For each of those 2 upper bounds, iterate over all 2^32 possible inputs to the RNG, verify the output is uniform.

* We note that the two above tests seem quite meaningful.
    For example, changing the `<=` comparison operator in [line 42 of Lemire_RNG.inl](https://github.com/nimia/Lemire_RNG/blob/master/Lemire_RNG.inl#L42) to `<` will bias the RNG
    in a very subtle way.
    The above tests actually catch this.

3. Compile the code with 64-bit word size. Choose an arbitrary upper bound for the RNG, and an arbitrary cutoff point.
  Generate many outputs of the RNG, and test how many of them are below the cutoff point.
  The result should be very close to `cutoff_point/random_upper_bound`.

  * We note that this test is not meaningful in the above sense.
    Making the above subtle change will not cause the test to break.

## Code-quality tools
* The 16-bit test is compiled with Clang [Memory Sanitizer](http://clang.llvm.org/docs/MemorySanitizer.html) and [Undefined Behavior Sanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html).
* The 32-bit test is compiled with [Address Sanitizer](http://clang.llvm.org/docs/AddressSanitizer.html) and Undefined Behavior Sanitizer.
* The 64-bit test is compiled with Memory Sanitizer and Undefined Behavior Sanitizer.
* In principle, since the 16- and 32-bit tests are exhaustive, they _"should"_ transfer to the 64-bit case.
  Obviously, those are famous last words. In particular, it is easy to write code that is correct as long as the word size is less than 64 bits, and exhibits undefined behavior on 64-bit.
However, since these tools do not report problems, this gives some shred of hope that the 16- and 32-bit tests indeed _"somewhat transfer"_ to the 64-bit case.
* Still, **YOU SHOULD NOT USE THIS CODE IN PRODUCTION**.

## TODOs / interesting future work
* The 64-bit test uses rand() as the underlying "random word generator".
  This is an intentional choice to allow reproduceable tests, by printing the seed at the start of each run; making the tests non-reproduceable would make it unclear how to debug a broken test.
  The sensible solution would be to instead use a strong deterministic RNG that can be initialized from a random seed, perhaps a stream cipher.
  However, I couldn't quickly locate an implementation of the above which is licensed under Apache 2, and decided to leave this as-is.
* Benchmark this implementation (with asserts disabled) vs. [the original 14-line implementation](https://lemire.me/blog/2019/06/06/nearly-divisionless-random-integer-generation-on-various-systems/).
