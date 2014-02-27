#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bootload.h"
#include "driver.h"

typedef int ( *LineHandler )( char *line );

#define BOOTLOAD_ON 0xFE
#define BOOTLOAD_OFF 0xFF

static int processMotorCommandLine( char *line );

int main( void ) {
    char *line = NULL;
    size_t size = 0;
    LineHandler lineHandler = processMotorCommandLine;

    if( !gpioInit() ) {
        exit( EXIT_FAILURE );
    }

    while( getline( &line, &size, stdin ) != -1 ) {
        switch( ( unsigned char )line[0] ) {
            case BOOTLOAD_ON:
                uartInit();
                lineHandler = processBootloadLine;
                break;
            case BOOTLOAD_OFF:
                uartClose();
                lineHandler = processMotorCommandLine;
                break;
        }
        if( !lineHandler( line ) ) {
            exit( EXIT_FAILURE );
        }
    }

    if( !gpioClose() ) {
        exit( EXIT_FAILURE );
    }

    free( line );
    exit( EXIT_SUCCESS );
}

static int processMotorCommandLine( char *line ) {
    char send[10], receive[10];
    size_t size = 0;

    memset( receive, '\0', sizeof( send ) );
    while( line[size] != '\n' ) {
        send[size] = line[size];
        size++;
    }
    printf( "%s\n", line );
    bcm2835_spi_transfernb( send, receive,  size );
    return 1;
}

