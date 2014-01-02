#include "driver.c"
#include "CuTest.h"

void startupTest(CuTest* tc) {
	CuAssert( tc, "Did not successfully initialize.", gpioInit() );
}

CuSuite* CuGetSuite(void) {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST( suite, startupTest );

	return suite;
}

