#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "bootload.h"
#include "comm.h"
#include "driver.h"

static int setCharByChar( void );
static int motorCommandLine( char commandType );
static int bootload( void );

int main( void ) {
    char character;

    if( !gpioInit() ) {
        exit( EXIT_FAILURE );
    }

    setCharByChar();

    while( read( 0, &character, 1 ) == 1 ) {
        if( character == Bootload ) {
            if( !uartInit() || !bootload() || !uartClose() ) {
                fprintf( stderr, "ERROR: Encountered problem while bootloading.\n" );
            }
        } else if( character == EndOfFile ) {
            break;
        } else {
            motorCommandLine( character );
        }
    }

    if( !gpioClose() ) {
        exit( EXIT_FAILURE );
    }

    exit( EXIT_SUCCESS );
}

static int setCharByChar( void ) {
    struct termios tio;

    if( tcgetattr( 0, &tio ) == -1 ) {
        printf( "Detected reading from pipe.\n" );
        return 1;
    }
    tio = tio;
    tio.c_lflag &= ~ICANON; /* Non-canonical mode */
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;
    if( tcsetattr( 0, TCSANOW, &tio ) == -1 ) {
        fprintf( stderr, "ERROR: Could not update the termios for stdin.\n" );
        return 0;
    }
    
    return 1;
}

static int motorCommandLine( char commandType ) {
    char command[sizeof( Command_t ) + 1];
    size_t i;

    command[0] = commandType;
    for( i = 1; i < sizeof( Command_t ); i++ ) {
        if( read( 0, &command[i], 1 ) == -1 ) {
            fprintf( stderr, "ERROR: Could not read a byte from stdin while processing command of type %d.\n", commandType );
            return 0;
        }
    }
    command[sizeof( Command_t )] = '\0';

    processMotorCommand( command );

    return 1;
}

static int bootload( void ) {
    char *line = NULL;
    size_t size = 0;
    int done = 0;

    while( getline( &line, &size, stdin ) != -1 || done ) {
        if( ( unsigned char )line[0] == Bootload ) {
            done = 1;
        } else if( !processBootloadLine( line ) ) {
            fprintf( stderr, "ERROR: Problem processing bootload line: %s.\n", line );
            return 0;
        }
    }

    free( line );
    return 1;
}

