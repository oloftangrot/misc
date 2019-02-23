#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
extern "C" {
#include "systime.h"
}

unsigned int sysTime(void)
{
	mock().actualCall("sysTime");
	return mock().returnUnsignedIntValueOrDefault(0);
}
