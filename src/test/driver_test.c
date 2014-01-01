#include "driver.c"
#include "CuTest.h"

void compileTest(CuTest* tc) {
	CuAssert( tc, "Everything compiles and runs!", 1 == 1 );
}

CuSuite* CuGetSuite(void) {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST( suite, compileTest );

	return suite;
}

