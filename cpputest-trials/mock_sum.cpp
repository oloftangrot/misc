#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
extern "C" {
#include "sum.h"
}

int sum(int a, int b) {
	mock().actualCall("sum")
	.withIntParameter("a", a)
	.withIntParameter("b", b);
	
	return mock().returnIntValueOrDefault(0);
}
