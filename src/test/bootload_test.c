#include "bootload.c"
#include "CuTest.h"

static int writeCount;
static uint8_t expectedBytes[4];

static void uartInitTest( CuTest* tc );
static void processBootloadLineTest( CuTest* tc );
static void uartCloseTest( CuTest* tc );

static void uartInitTest( CuTest* tc ) {
    CuAssert( tc, "Did not successfully set up for UART communication.", uartInit() );
}

static void processBootloadLineTest( CuTest* tc ) {
    writeCount = 0;
    expectedBytes[0] = 0x00;
    expectedBytes[1] = 0xFF;
    expectedBytes[2] = 0x55;
    expectedBytes[3] = 0xAA;
    CuAssert( tc, "Did not process data line correctly.", processBootloadLine( "00 FF 55 AA " ) );
    CuAssert( tc, "Did not process all of the lines.", writeCount == 4 );
    CuAssert( tc, "Did not process last line correctly.", processBootloadLine( "q" ) );
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

#ifdef TEST
static int sendByte( uint8_t byte ) {
    if( byte != expectedBytes[writeCount] ) {
        return 0;
    } else {
        writeCount++;
        return 1;
    }
}
#endif

