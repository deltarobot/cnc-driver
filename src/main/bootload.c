#include <stdlib.h>
#include "bootload.h"

#define UART "/dev/ttyAMA0"

int uartInit( void ) {
    return 1;
}

int uartClose( void ) {
    return 1;
}

int processBootloadLine( char *line ) {
    if( line != NULL ) {
        return 1;
    } else {
        return 0;
    }
}

