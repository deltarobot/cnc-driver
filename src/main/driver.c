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

