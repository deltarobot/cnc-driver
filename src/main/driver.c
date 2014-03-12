#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "comm.h"
#include "driver.h"
#include "bcm2835.h"

#define MAX_RETRIES 5

static void setupUartPins( void );
static void setupSpiPins( void );

int gpioInit( void ) {
    if( !bcm2835_init() ) {
        return 0;
    }

    setupUartPins();
    setupSpiPins();

    return 1;
}

int gpioClose( void ) {
    bcm2835_spi_end();
    return bcm2835_close();
}

int processMotorCommand( char *command ) {
    char receive[sizeof( Command_t ) + 1];
    int successful = 0, i;

    for( i = 0; i < MAX_RETRIES && !successful; i++ ) {
        memset( receive, '\0', sizeof( receive ) );

        bcm2835_spi_transfernb( command, receive, sizeof( Command_t ) + 1 );
        if( memcmp( command, &receive[1], sizeof( Command_t ) ) == 0 ) {
            successful = 1;
        }
    }

    if( !successful ) {
        fprintf( stderr, "ERROR: Gave up on sending command after multiple attempts.\nCommand was {%s}.\n", command );
    }
    return successful;
}

static void setupUartPins( void ) {
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_08, BCM2835_GPIO_FSEL_ALT0 );
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_10, BCM2835_GPIO_FSEL_ALT0 );
}

static void setupSpiPins( void ) {
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder( BCM2835_SPI_BIT_ORDER_MSBFIRST );
    bcm2835_spi_setDataMode( BCM2835_SPI_MODE0 );
    bcm2835_spi_setClockDivider( BCM2835_SPI_CLOCK_DIVIDER_65536 );
    bcm2835_spi_chipSelect( BCM2835_SPI_CS0 );
    bcm2835_spi_setChipSelectPolarity( BCM2835_SPI_CS0, LOW );
}

