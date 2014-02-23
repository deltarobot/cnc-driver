#include <stdio.h>
#include "driver.h"

int main( int argc, char *argv[] ) {
    GpioType gpioType = NORMAL;

    if( argc > 1 && strcmp( argv[1], "-b" ) == 0 ) {
        gpioType = BOOTLOAD;
    }

    if( !gpioInit( gpioType ) ) {
        return -1;
    }

    return 0;
}