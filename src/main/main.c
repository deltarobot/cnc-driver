#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "bootload.h"
#include "comm.h"
#include "driver.h"

static char *readPipe = NULL;

static int startupAndConfigure( int argc, char *argv[] );
static int openReadPipe( void );
static int setCharByChar( void );
static int motorCommandLine( char commandType );
static int bootload( void );

int main( int argc, char *argv[] ) {
    char character;

    if( !gpioInit() ) {
        exit( EXIT_FAILURE );
    }

    if( !startupAndConfigure( argc, argv ) ) {
        exit( EXIT_FAILURE );
    }

    for( ;; ) {
        if( read( 0, &character, 1 ) == 0 ) {
            if( !openReadPipe() ) {
                exit( EXIT_FAILURE );   
            }
            continue;
        }
        if( character == Bootload ) {
            if( !uartInit() || !bootload() || !uartClose() ) {
                fprintf( stderr, "ERROR: Encountered problem while bootloading.\n" );
            }
            printf( "Completed bootload.\n" );
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

static int startupAndConfigure( int argc, char *argv[] ) {
    int i;

    for( i = 1; i < argc; i++ ) {
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
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
        return setCharByChar();
    }
    
    fd = open( readPipe, O_RDONLY );

    if( fd == -1 ) {
        fprintf( stderr, "Could not open pipe at %s.\n", readPipe );
        return 0;
    }

    close( 0 );
    dup( fd );
    close( fd );

    fprintf( stderr, "Opened pipe at %s.\n", readPipe );
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

    while( getline( &line, &size, stdin ) != -1 && !done ) {
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

