#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "bootload.h"

#define UART "/dev/ttyAMA0"
int fd = -1;

int uartInit( void ) {
    fd = open( UART, O_RDWR | O_NONBLOCK | O_NOCTTY );
    if( fd == -1 ) {
        return 0;
    } else {
        return 1;
    }
}

int uartClose( void ) {
    if( close( fd ) == -1 ) {
        return 0;
    } else {
        return 1;
    }
}

int processBootloadLine( char *line ) {
    if( line != NULL ) {
        return 1;
    } else {
        return 0;
    }
}

