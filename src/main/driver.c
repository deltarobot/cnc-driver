#include "driver.h"
#include "bcm2835.h"

#define PWM_CHANNEL 0
#define RANGE 1024

int gpioInit( GpioType gpioType ) {
    if( !bcm2835_init() ) {
        return 0;
    }

    switch( gpioType ) {
        case NORMAL:
            break;
        case BOOTLOAD:
            bcm2835_gpio_fsel( RPI_V2_GPIO_P1_08, BCM2835_GPIO_FSEL_ALT0 );
            bcm2835_gpio_fsel( RPI_V2_GPIO_P1_10, BCM2835_GPIO_FSEL_ALT0 );
            break;
        default:
            return 0;
    }

    return 1;
}

int gpioClose( void ) {
    return bcm2835_close();
}

