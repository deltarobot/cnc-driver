#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bootload.h"
#include "driver.h"

int main( int argc, char *argv[] ) {
    char *line = NULL;
    size_t size = 0;
    GpioType gpioType = NORMAL;

    if( argc > 1 && strcmp( argv[1], "-b" ) == 0 ) {
        gpioType = BOOTLOAD;
    }

    if( !gpioInit( gpioType ) ) {
        exit( EXIT_FAILURE );
    }

    if( gpioType == BOOTLOAD ) {
        uartInit();
        while( getline( &line, &size, stdin ) != -1 ) {
            if( !processBootloadLine( line ) ) {
                exit( EXIT_FAILURE );
            }
        }
        uartClose();
    }

    free( line );
    exit( EXIT_SUCCESS );
}

