#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "bootload.h"

#define UART "/dev/ttyAMA0"
static int fd = -1;

static int sendByte( uint8_t byte );

int uartInit( void ) {
    struct termios tio, newTio;

    fd = open( UART, O_RDWR | O_NONBLOCK | O_NOCTTY );
    if( fd == -1 ) {
        fprintf( stderr, "ERROR: Couldn't open the terminal" );
        return 0;
    }

    if( tcgetattr( fd, &tio ) == -1 ) {
        return 0;
    }

    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag = CS8 | CREAD | CLOCAL;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;
    cfsetospeed( &tio, B9600 );
    cfsetispeed( &tio, B9600 );
    tcsetattr( fd, TCSANOW, &tio );

    if( tcgetattr( fd, &newTio ) == -1 ) {
        return 0;
    }

    if ( memcmp( &tio, &newTio, sizeof( tio ) ) != 0 ) {
        fprintf( stderr, "ERROR: Terminal changes were not applied" );
        return 0;
    }

    return 1;
}

int uartClose( void ) {
    if( close( fd ) == -1 ) {
        fd = -1;
        return 0;
    } else {
        return 1;
    }
}

int processBootloadLine( char *line ) {
    long int converted;

    if( *line == 'q' ) {
        return 1;
    }

    while( *line != '\0' ) {
        converted = strtol( line, NULL, 16 );
        if( converted == LONG_MIN || converted == LONG_MAX ) {
            return 0;
        }
        if( !sendByte( ( uint8_t )( converted & 0xFF ) ) ) {
            return 0;
        }
        line += 3;
    }

    return 1;
}

#ifndef TEST
static int sendByte( uint8_t byte ) {
    return 1;
}
#endif

