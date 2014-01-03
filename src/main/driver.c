#include "driver.h"
#include "bcm2835.h"

int gpioInit( void ) {
    if( !bcm2835_init() ) {
        return 0;
    }

    return 1;
}

