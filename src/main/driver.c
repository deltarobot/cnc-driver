#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "comm.h"
#include "driver.h"
#include "bcm2835.h"

#define MAX_RETRIES 5

static void setupResetPin( void );
static void setupUartPins( void );
static void setupSpiPins( void );

int gpioInit( void ) {
    if( !bcm2835_init() ) {
        return 0;
    }

    setupResetPin();
    setupUartPins();
    setupSpiPins();

    return 1;
}

int gpioClose( void ) {
    bcm2835_spi_end();
    return bcm2835_close();
}

int processMotorCommand( char *command ) {
    char receive[sizeof( Command_t ) + 2];
    int successful = 0;
    size_t i;

    for( i = 0; i < MAX_RETRIES && !successful; i++ ) {
        memset( receive, '\0', sizeof( receive ) );

        bcm2835_spi_transfernb( command, receive, sizeof( receive ) );
        if( memcmp( command, receive + 2, sizeof( Command_t ) ) == 0 ) {
            successful = 1;
        }
    }
    printf( "Command sent after %d attempt(s).\n", i );

    if( !successful ) {
        fprintf( stderr, "ERROR: Gave up on sending command after multiple attempts.\n" );
        for( i = 0; i < sizeof( Command_t ); i++ ) {
            printf( "Sent: %02x, Got: %02x\n", command[i], receive[i + 2] );
        }
    }
    return successful;
}

int resetController( void ) {
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_07, BCM2835_GPIO_FSEL_OUTP );
    bcm2835_gpio_write( RPI_V2_GPIO_P1_07, LOW );
    bcm2835_delay( 1 );
    setupResetPin();
    bcm2835_delay( 1 );
    return 1;
}

static void setupResetPin( void ) {
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_07, BCM2835_GPIO_FSEL_INPT );
}

static void setupUartPins( void ) {
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_08, BCM2835_GPIO_FSEL_ALT0 );
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_10, BCM2835_GPIO_FSEL_ALT0 );
}

static void setupSpiPins( void ) {
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder( BCM2835_SPI_BIT_ORDER_MSBFIRST );
    bcm2835_spi_setDataMode( BCM2835_SPI_MODE0 );
    bcm2835_spi_setClockDivider( BCM2835_SPI_CLOCK_DIVIDER_128 );
    bcm2835_spi_chipSelect( BCM2835_SPI_CS0 );
    bcm2835_spi_setChipSelectPolarity( BCM2835_SPI_CS0, LOW );
}

