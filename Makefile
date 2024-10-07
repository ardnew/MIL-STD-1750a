PROJECT = milstd1750a

LIBRARY = lib$(PROJECT).a
TEST_BINARY = test_$(PROJECT)

SRC = $(wildcard src/*.c)
TEST_SRC = $(wildcard test/*.c)

OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)

# Configure toolchain selection based on OS/env
ifneq (Darwin,$(shell uname -s))
  # Default is GNU GCC (excluding macOS)
  CC = gcc
  DBG = gdb
  DBGFLAGS = -ggdb -Og
else
  # macOS uses LLVM/Clang (unless $ALTTOOLCHAIN is set)
  ifeq (,$(ALTTOOLCHAIN))
    CC = clang
    DBG = lldb
    DBGFLAGS = -g -O0
  else
    # Note that macOS reports "arm64" (uname, arch, etc.)
    # instead of Arm's "aarch64" (per ABI/ISA spec).
    # So there isn't a sane way to query these automatically.
    CC = aarch64-apple-darwin24-gcc-14
    DBG = lldb
    DBGFLAGS = -g -O0
  endif
endif

CFLAGS = -Wall -Wextra -Werror -Wno-sign-compare -pedantic -Iinclude -std=gnu2x
LDLIBS = -lm
LDFLAGS = -L.

all: $(LIBRARY) $(TEST_BINARY)

$(LIBRARY): $(OBJ)
	ar rcs $@ $^

$(TEST_BINARY): $(TEST_OBJ) $(LIBRARY)
	$(CC) $(DBGFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(DBGFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ) $(TEST_OBJ) $(LIBRARY) $(TEST_BINARY)

test: $(TEST_BINARY)
	./$(TEST_BINARY)

debug: $(TEST_BINARY)
	$(DBG) ./$(TEST_BINARY)

.PHONY: all clean test debug
