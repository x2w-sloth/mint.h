#define MINT_IMPLEMENTATION
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

int main(int argc, char *argv[]) {
	return mint_main(argc, argv);
}
