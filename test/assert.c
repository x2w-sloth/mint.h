#define MINT_IMPLEMENTATION
#include "mint.h"

UNIT_TEST(assert_ok)
{
	CHECK(1);
	ASSERT(1);
	CHECK(1);
}

UNIT_TEST(assert_fail)
{
	CHECK(1);
	ASSERT(0); // abort
	CHECK(0);
	ASSERT(0);
}

int main(int argc, char **argv)
{
	return mint_main(argc, argv);
}
