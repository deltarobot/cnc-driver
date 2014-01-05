#include <stdio.h>
#include <stdlib.h>
#include "driver.c"
#include "CuTest.h"

#define TEST_PINS 17

static RPiGPIOPin pins[TEST_PINS] = {
    RPI_V2_GPIO_P1_03,
    RPI_V2_GPIO_P1_05,
    RPI_V2_GPIO_P1_07,
    RPI_V2_GPIO_P1_08,
    RPI_V2_GPIO_P1_10,
    RPI_V2_GPIO_P1_11,
    RPI_V2_GPIO_P1_12,
    RPI_V2_GPIO_P1_13,
    RPI_V2_GPIO_P1_15,
    RPI_V2_GPIO_P1_16,
    RPI_V2_GPIO_P1_18,
    RPI_V2_GPIO_P1_19,
    RPI_V2_GPIO_P1_21,
    RPI_V2_GPIO_P1_22,
    RPI_V2_GPIO_P1_23,
    RPI_V2_GPIO_P1_24,
    RPI_V2_GPIO_P1_26
};

void startupTest( CuTest* tc ) {
    bcm2835_set_debug( 1 );

    CuAssert( tc, "Did not successfully initialize.", gpioInit() );
}

void testAllPins( CuTest* tc ) {
    int i;
    size_t size = 4;
    char *buffer = malloc( size );

    for( i = 0; i < TEST_PINS; ++i ) {
        bcm2835_gpio_fsel( pins[i], BCM2835_GPIO_FSEL_OUTP );
        bcm2835_gpio_write( pins[i], LOW );
    }

    for( i = 0; i < TEST_PINS; ++i ) {
        printf( "i = %d, GPIO = %d\nPress enter to continue", i, pins[i] );
        if( getline( &buffer, &size, stdin ) == -1 ) {
            printf( "No line.\n" );
        }

        bcm2835_gpio_write( pins[i], LOW );
        bcm2835_delay( 500 );
        bcm2835_gpio_write( pins[i], HIGH );
        bcm2835_delay( 500 );
        bcm2835_gpio_write( pins[i], LOW );
        bcm2835_delay( 500 );
        bcm2835_gpio_write( pins[i], HIGH );
        bcm2835_delay( 500 );
    }

    CuAssert( tc, "Drove all pins successfully.", 1 );

    free( buffer );
}

CuSuite* CuGetSuite( void ) {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST( suite, startupTest );
	SUITE_ADD_TEST( suite, testAllPins );

    return suite;
}
