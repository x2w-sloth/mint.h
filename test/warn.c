#define MINT_IMPLEMENTATION
#include "mint.h"

UNIT_TEST(warn_1)
{
	WARN(1);
	WARN(1);
	WARN(1);
	WARN(1);
}

UNIT_TEST(warn_0)
{
	// warnings do not fail the test
	WARN(0 > 0);
	WARN(0 < 0);
}

int main(int argc, char **argv)
{
	return mint_main(argc, argv);
}
