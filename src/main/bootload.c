#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "bootload.h"
#include "driver.h"

#define UART "/dev/ttyAMA0"
static int fd = -1;

static int readHexByte( char *line, uint8_t *byte );
static int sendByte( uint8_t byte );
static int autoBaud( void );

int uartInit( void ) {
    struct termios tio, newTio;

    fd = open( UART, O_RDWR | O_NONBLOCK | O_NOCTTY );
    if( fd == -1 ) {
        fprintf( stderr, "ERROR: Couldn't open the terminal.\n" );
        return 0;
    }

    memset( &tio, 0, sizeof( tio ) );
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
        fprintf( stderr, "WARNING: Terminal changes were not fully applied.\n" );
    }

    if( !autoBaud() ) {
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
    uint8_t upperByte, lowerByte;

    if( *line == 'q' ) {
        return 1;
    }

    while( *line != '\0' && *line != '\r' && *line != '\n' ) {
        if( !readHexByte( line, &upperByte ) || !readHexByte( line + 3, &lowerByte ) ) {
            return 0;
        }
        if( !sendByte( lowerByte ) || !sendByte( upperByte ) ) {
            fprintf( stderr, "ERROR: Problem sending the byte over UART.\n" );
            return 0;
        }
        line += 6;
    }

    return 1;
}

static int readHexByte( char *line, uint8_t *byte ) {
    long int converted;

    if( *line == '\0' ) {
        fprintf( stderr, "ERROR: Reached the end of the line, odd number of bytes?\n" );
        return 0;
    }

    converted = strtol( line, NULL, 16 );
    if( converted == LONG_MIN || converted == LONG_MAX ) {
        fprintf( stderr, "ERROR: Could not read the hex line, %s.\n", line );
        return 0;
    }

    *byte = ( uint8_t )( converted & 0xFF );
    return 1;
}

#ifndef TEST
static int autoBaud( void ) {
    char autoBaud = 'A', readByte = '\0';
    int complete = 0, attempts = 1;

    printf( "Beginning Autobaud.\n" );
    while( !complete ) {
        if( ( attempts - 1 ) % 5 == 0 ) {
            printf( "Resetting the microcontroller.\n" );
            resetController();
        }
        printf( "Autobaud attempt %d.\n", attempts++ );
        if( write( fd, &autoBaud, 1 ) == -1 ) {
            fprintf( stderr, "ERROR: Could not write to the UART.\n" );
            return 0;
        }
        sleep( 1 );
        if( read( fd, &readByte, 1 ) == 1 ) {
            if( autoBaud == readByte ) {
                complete = 1;
            }
        }
    }
    printf( "Autobaud finished successfully.\n" );

    /* Make file reads blocking again. */
    fcntl( fd, F_SETFL, 0 );
    return 1;
}

static int sendByte( uint8_t byte ) {
    uint8_t readByte;

    if( write( fd, &byte, 1 ) == -1 ) {
        fprintf( stderr, "ERROR: Could not write to the UART.\n" );
        return 0;
    }
    if( read( fd, &readByte, 1 ) != 1 ) {
        fprintf( stderr, "ERROR: Did not get any response back from the UART.\n" );
        return 0;
    }
    if( byte != readByte ) {
        fprintf( stderr, "ERROR: Response was not an echo of the write. Expected 0x%02x but got 0x%02x.\n", byte, readByte );
        return 0;
    }

    return 1;
}
#endif

