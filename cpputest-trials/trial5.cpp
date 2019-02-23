#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
extern "C" {
#include "bar.h"
}

TEST_GROUP(bar)
{
};

TEST(bar, FirstTest)
{
   mock().expectOneCall("sysTime")
	   .andReturnValue(11);

   int res;  
   res = bar();
   CHECK_EQUAL(11, res);
}


