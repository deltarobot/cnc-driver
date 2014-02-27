#include "driver.h"
#include "bcm2835.h"

#define PWM_CHANNEL 0
#define RANGE 1024

int gpioInit( void ) {
    if( !bcm2835_init() ) {
        return 0;
    }

    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_08, BCM2835_GPIO_FSEL_ALT0 );
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_10, BCM2835_GPIO_FSEL_ALT0 );

    return 1;
}

int gpioClose( void ) {
    return bcm2835_close();
}

