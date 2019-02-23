#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
extern "C" {
#include "foo.h"
}

TEST_GROUP(foo)
{
};

TEST(foo, FirstTest)
{
   int res;  
   res = foo(3,5);
	CHECK_EQUAL(8, res);
}

TEST(foo, SecondTest)
{
   mock().expectOneCall("sum")
	   .withParameter("a",6)
	   .withParameter("b",5)
	   .andReturnValue(11);

   int res;  
   res = foo(3,5);
   CHECK_EQUAL(11, res);
}


