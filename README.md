# mint.h

A minimal unit testing header file for C, focusing on simplicity and readability.

Mint is written in C to test C, there are no exceptions, templates, or other artifacts. Each unit test is a modest C function. Instead of having dozens of test macros to print different error messages for each data type, Mint only provides 3 test macros and summarizes final test results concisely.

## Usage

Include `mint.h` wherever you need to write unit tests. Declare a unit test with the `UNIT_TEST` macro and start testing with `CHECK`, `WARN`, and `ASSERT`s. On failure, the `WARN` macro prints error messages but does not mark the current unit test as failed, while the `ASSERT` macro aborts the unit test immediately.

```c
#include "mint.h"

static int add(int a, int b) {
	return a + b;
}

static int sub(int a, int b) {
	return a - b;
}

UNIT_TEST(test_add) {
	CHECK(add(1, 2) == 3);
	CHECK(add(2, 3) == 5);
    CHECK(add(3, 4) == 7);
}

UNIT_TEST(test_sub) {
	CHECK(sub(4, 1) == 3);
	CHECK(sub(1, 3) == -2);
}
```

After writing some tests, define `MINT_IMPLEMENTATION` before including `mint.h` in the main source file of your test driver, this gives you the `mint_main` function, call it to run your unit tests.

```c
#define MINT_IMPLEMENTATION
#include "mint.h"

int main(int argc, char *argv[]) {
	return mint_main(argc, argv);
}
```

Finally, compile with any other source files that have `UNIT_TEST`s in it to register them in the resulting test binary.

## Output

By default Mint outputs colored text to `stdout` from the test binary.

```txt
[#] running 2 units
[+] test_add (unit 1/2)
[+] test_add passed
[+] test_sub (unit 2/2)
[+] test_sub passed
[#] total 2/2 units passed
[#] total 5 tests passed
```

## License

This repository is distrubuted under the permissive MIT License.
