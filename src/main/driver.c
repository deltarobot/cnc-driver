#include <stdio.h>
#include "driver.h"
#include "bcm2835.h"

#define PWM_CHANNEL 0
#define RANGE 1024

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

int processMotorCommandLine( char *line ) {
    size_t bufferSize = 10, size = 0;
    char send[bufferSize], receive[bufferSize];
    int i;

    memset( send, '\0', bufferSize );
    memset( receive, '\0', bufferSize );
    while( line[size] != '\n' && size < bufferSize - 1 ) {
        send[size] = line[size];
        size++;
    }
    send[size] = '\0';
    size++;
    bcm2835_spi_transfernb( send, receive,  size );

    if( memcmp( send, receive + 1, size - 1 ) == 0 ) {
        return 1;
    } else {
        fprintf( stderr, "ERROR: No echo back on SPI from the controller.\n" );
        for( i = 0; i < size - 1; i++ ) {
            printf( "Sent: %02x, Got: %02x\n", send[i], receive[i + 1] );
        }
        return 0;
    }
}

static void setupUartPins( void ) {
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_08, BCM2835_GPIO_FSEL_ALT0 );
    bcm2835_gpio_fsel( RPI_V2_GPIO_P1_10, BCM2835_GPIO_FSEL_ALT0 );
}

static void setupSpiPins( void ) {
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder( BCM2835_SPI_BIT_ORDER_MSBFIRST );
    bcm2835_spi_setDataMode( BCM2835_SPI_MODE0 );
    bcm2835_spi_setClockDivider( BCM2835_SPI_CLOCK_DIVIDER_256 );
    bcm2835_spi_chipSelect( BCM2835_SPI_CS0 );
    bcm2835_spi_setChipSelectPolarity( BCM2835_SPI_CS0, LOW );
}

