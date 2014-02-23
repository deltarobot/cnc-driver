#include "bootload.c"
#include "CuTest.h"

static void uartInitTest( CuTest* tc );
static void processBootloadLineTest( CuTest* tc );
static void uartCloseTest( CuTest* tc );

static void uartInitTest( CuTest* tc ) {
    CuAssert( tc, "Did not successfully set up for UART communication.", uartInit() );
}

static void processBootloadLineTest( CuTest* tc ) {
    CuAssert( tc, "Did not process the string correctly.", processBootloadLine( "00" ) );
}

static void uartCloseTest( CuTest* tc ) {
    CuAssert( tc, "Did not successfully close the UART communication.", uartClose() );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, uartInitTest );
    SUITE_ADD_TEST( suite, processBootloadLineTest );
    SUITE_ADD_TEST( suite, uartCloseTest );

    return suite;
}

