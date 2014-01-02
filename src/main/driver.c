#include "driver.h"
#include "bcm2835.h"

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

int gpioInit( void ) {
    int i;

    bcm2835_set_debug( 1 );

    if( !bcm2835_init() ) {
        return 0;
    }

    // Configure some GPIO pins fo some testing
    // Set all the pins to outputs
    for( i = 0; i < TEST_PINS; ++i ) {
        bcm2835_gpio_fsel( pins[i], BCM2835_GPIO_FSEL_OUTP );
    }

    for( i = 0; i < TEST_PINS; ++i ) {
        bcm2835_gpio_write( pins[i], LOW );
        bcm2835_delay( 500 );
        bcm2835_gpio_write( pins[i], HIGH );
        bcm2835_delay( 500 );
        bcm2835_gpio_write( pins[i], LOW );
        bcm2835_delay( 500 );
        bcm2835_gpio_write( pins[i], HIGH );
        bcm2835_delay( 500 );
    }

    if ( !bcm2835_close() ) {
        return 0;
    }

    return 1;
}

