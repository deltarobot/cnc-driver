#include <stdio.h>
#include "driver.h"

int main( void ) {
    if( !gpioInit() ) {
        return -1;
    }

    return 0;
}