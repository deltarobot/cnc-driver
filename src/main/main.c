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

    gpioClose();

    free( line );
    exit( EXIT_SUCCESS );
}

static int processMotorCommandLine( char *line ) {
    printf( "%s\n", line );
    return 1;
}

