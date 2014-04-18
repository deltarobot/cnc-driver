#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include "bootload.h"
#include "comm.h"
#include "driver.h"

#define EXTRA_BYTES 2
#define MAX_PACKET_SIZE sizeof( Command_t ) * 3 + EXTRA_BYTES

static char *readPipe = NULL;
static int bootloadMode = 0;

static int startupAndConfigure( int argc, char *argv[] );
static int openReadPipe( void );
static int setCharByChar( void );
static int motorCommandLine( void );
static int bootload( void );

int main( int argc, char *argv[] ) {
    if( !gpioInit() ) {
        exit( EXIT_FAILURE );
    }

    if( !startupAndConfigure( argc, argv ) ) {
        exit( EXIT_FAILURE );
    }

    for( ;; ) {
        if( bootloadMode ) {
            if( !uartInit() || !bootload() || !uartClose() ) {
                fprintf( stderr, "ERROR: Encountered problem while bootloading.\n" );
            }
            printf( "Completed bootload.\n" );
        } else {
            if( !motorCommandLine() ) {
                fprintf( stderr, "ERROR: Encountered problem while processing command line.\n" );
            }
        }
        if( !openReadPipe() ) {
            exit( EXIT_FAILURE );
        }
    }

    if( !gpioClose() ) {
        exit( EXIT_FAILURE );
    }

    exit( EXIT_SUCCESS );
}

static int startupAndConfigure( int argc, char *argv[] ) {
    int i;

    for( i = 1; i < argc; i++ ) {
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
                case 'b':
                    bootloadMode = 1;
                    break;
                case 'r':
                    readPipe = argv[i + 1];
                    break;
                default:
                    fprintf( stderr, "ERROR: Unknown argument: %s\n", argv[i] );
                    return 0;
            }
        }
    }

    if( !openReadPipe() ) {
        return 0;
    }

    return 1;
}

static int openReadPipe( void ) {
    int fd;

    if( readPipe == NULL ) {
        fprintf( stderr, "No pipe to open, using stdin.\n" );
        if( bootloadMode ) {
            return 1;
        } else {
            return setCharByChar();
        }
    }
    
    fd = open( readPipe, O_RDONLY );

    if( fd == -1 ) {
        fprintf( stderr, "Could not open pipe at %s.\n", readPipe );
        return 0;
    }

    close( 0 );
    dup( fd );
    close( fd );

    printf( "Opened pipe at %s.\n", readPipe );
    return 1;
}

static int setCharByChar( void ) {
    struct termios tio;

    if( tcgetattr( 0, &tio ) == -1 ) {
        fprintf( stderr, "ERROR: Could not get termios for stdin.\n" );
        return 0;
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

static int motorCommandLine( void ) {
    char command[MAX_PACKET_SIZE];
    char echoBack[MAX_PACKET_SIZE];
    char numberCommands;
    size_t i, j, actualSize;
    struct timespec time;

    if( read( 0, &numberCommands, 1 ) != 1 ) {
        return 0;
    }

    actualSize = numberCommands * sizeof( Command_t );

    for( i = 0; i < actualSize / 2; i++ ) {
        for( j = 0; j < 2; j++ ) {
            if( read( 0, &command[i * 2 + 1 - j], 1 ) != 1 ) {
                fprintf( stderr, "ERROR: Could not read a byte from stdin while processing command of type %d.\n", command[0] );
                return 0;
            }
        }
    }
    for( i = actualSize; i < MAX_PACKET_SIZE; i++ ) {
        command[i] = 0;
    }
    printf( "Got %d command(s) to send.\n", numberCommands );

    processMotorCommand( command, echoBack, numberCommands, actualSize, EXTRA_BYTES );

    if( read( 0, &time, sizeof( struct timespec ) ) != sizeof( struct timespec ) ) {
        fprintf( stderr, "ERROR: Could not read a byte from stdin while processing command of type %d.\n", command[0] );
        return 0;
    }
    nanosleep( &time, NULL );

    return 1;
}

static int bootload( void ) {
    char *line = NULL;
    size_t size = 0;
    int done = 0;

    while( !done ) {
        if( getline( &line, &size, stdin ) == 0 ) {
            fprintf( stderr, "ERROR: Got to end of file while bootloading." );
            return 0;
        }
        if( *line == 'q' ) {
            done = 1;
        } else if( !processBootloadLine( line ) ) {
            fprintf( stderr, "ERROR: Problem processing bootload line: %s.\n", line );
            return 0;
        }
    }

    free( line );
    return 1;
}

