#include "driver.h"
#include "bcm2835.h"

int gpioInit( void ) {
    int i;

    bcm2835_set_debug( 1 );

    if( !bcm2835_init() ) {
        return 0;
    }

    // Configure some GPIO pins fo some testing
    // Set RPI pin P1-11 to be an output
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_OUTP );
    // and input hysteresis disabled on GPIOs 0 to 27
    bcm2835_gpio_set_pad( BCM2835_PAD_GROUP_GPIO_0_27, BCM2835_PAD_SLEW_RATE_UNLIMITED | BCM2835_PAD_DRIVE_8mA );

    for( i = 0; i < 5; ++i ) {
        bcm2835_gpio_write( RPI_V2_GPIO_P1_11, HIGH );
        bcm2835_delay( 500 );
        bcm2835_gpio_write( RPI_V2_GPIO_P1_11, LOW );
        bcm2835_delay( 500 );
    }

    if ( !bcm2835_close() ) {
        return 0;
    }

    return 1;
}

