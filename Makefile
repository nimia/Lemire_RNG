EXECUTABLES=test_16_bit test_32_bit test_64_bit
OBJECTS=Lemire_RNG.o
COMPILE_COMMANDS=compile_commands.json
CFLAGS=-O3 -g -Wall -Wextra
TEST_CFLAGS_16_BIT=-fsanitize=memory,undefined
# memory sanitizer is too expensive in 32-bit mode
TEST_CFLAGS_32_BIT=-fsanitize=address,undefined
TEST_CFLAGS_64_BIT=-fsanitize=memory,undefined
CC=clang

.PHONY: all
all: $(EXECUTABLES) $(OBJECTS)

Lemire_RNG.o: Lemire_RNG.c word_size.h Lemire_RNG.inl
	$(CC) $(CFLAGS) -DWORD_SIZE_BITS=64 -c $<

test_16_bit: test_16_bit.c word_size.h Lemire_RNG.inl test_util.inl
	$(CC) $(CFLAGS) $(TEST_CFLAGS_16_BIT) $< -o $@

# dev_urandom_utils cannot be compiled separately into an object, since it
# depends on the value passed to -DWORD_SIZE_BITS.
test_32_bit: test_32_bit.c word_size.h Lemire_RNG.inl test_util.inl \
	dev_urandom_utils.h dev_urandom_utils.c
	$(CC) $(CFLAGS) $(TEST_CFLAGS_32_BIT) -DWORD_SIZE_BITS=32 $< \
	dev_urandom_utils.c -o $@

# Similarly, Lemire_RNG.c depends on the word size and also cannot be compiled
# separately into an object.
test_64_bit: test_64_bit.c word_size.h Lemire_RNG.inl Lemire_RNG.h Lemire_RNG.c \
	dev_urandom_utils.h dev_urandom_utils.c
	$(CC) $(CFLAGS) $(TEST_CFLAGS_64_BIT) -DWORD_SIZE_BITS=64 test_64_bit.c \
	Lemire_RNG.c dev_urandom_utils.c -o test_64_bit

.PHONY: clean
clean:
	-rm -f $(EXECUTABLES) $(OBJECTS) $(COMPILE_COMMANDS)

.PHONY: test
test: $(EXECUTABLES)
	./test_16_bit
	./test_32_bit
	./test_64_bit

.PHONY: quick_test
quick_test: test_16_bit
	./test_16_bit 65000

.PHONY: clang_format
clang_format:
	clang-format -i *.c *.h *.inl

# Technically, compile_commands.json depends even on the makefile itself.
# I would rather designate it as phony and be sure it is up-to-date when built,
# rather than doing weird circular dependencies in make.
.PHONY: compile_commands.json
compile_commands.json:
	make clean
	bear make all

.PHONY: clang_tidy
clang_tidy: compile_commands.json
	./run-clang-tidy.py -checks=clang-analyzer-*,bugprone-*,readability-* \
	-header-filter=.* -quiet
