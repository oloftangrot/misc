#include "CppUTest/TestHarness.h"
extern "C" {
#include "sum.h"
}

TEST_GROUP(sum)
{
};

TEST(sum, FirstTest)
{
   int res;  
   res = sum(3,5);
	CHECK_EQUAL(8, res);
}



