#include "driver.h"
#include "bcm2835.h"

#define PWM_CHANNEL 0
#define RANGE 1024

int gpioInit( void ) {
    if( !bcm2835_init() ) {
        return 0;
    }

    return 1;
}

int gpioClose( void ) {
    return bcm2835_close();
}

