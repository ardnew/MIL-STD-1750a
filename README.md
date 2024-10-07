# MIL-STD-1750a
#### Encode/decode MIL-STD-1750a floating-point
##### See [specification](doc/MIL-STD-1750A.PDF)

## Usage

The [`Makefile`](Makefile) demonstrates how to compile, link, test, and debug
on a variety of targets. It will use `gcc`/`gdb` by default, or, if it detects
the host is macOS, it will use `clang`/`lldb` instead.

The following is output when testing the package on an Apple Macbook M3 Pro:

```sh
$ make test
clang -g -O0 -Wall -Wextra -Werror -Wno-sign-compare -pedantic -Iinclude -std=gnu2x  -c -o test/test_encdec.o test/test_encdec.c
clang -g -O0 -Wall -Wextra -Werror -Wno-sign-compare -pedantic -Iinclude -std=gnu2x  -c -o src/encdec.o src/encdec.c
ar rcs libmilstd1750a.a src/encdec.o
clang -g -O0 -L. test/test_encdec.o libmilstd1750a.a -lm -o test_milstd1750a
./test_milstd1750a
All tests passed.
```

Use the `make debug` target to rebuild and load the test executable in your debugger.

