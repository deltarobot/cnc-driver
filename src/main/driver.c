#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "driver.h"
#include "bcm2835.h"
#include <inttypes.h>

#define ECHO_DELAY 1

static uint16_t gpoData;

static void writeLcdCommand( uint8_t data );
static void writeLcdData( uint8_t data );
static void sendGpoData( void );
static void setupResetPin( void );
static void setupUartPins( void );
static void setupSpiPins( void );
static void setupI2c( void );
static void setupLcd( void );

int gpioInit( void ) {
    if( !bcm2835_init() ) {
        return 0;
    }
    
    setupResetPin();
    setupUartPins();
    setupSpiPins();
    setupI2c();
    setupLcd();
    writeString( "CEENC" );

    return 1;
}

int gpioClose( void ) {
    bcm2835_spi_end();
    bcm2835_i2c_end();
    return bcm2835_close();
}

int processMotorCommand( char *command, char *receive, int size, int extraBytes ) {
    int successful = 0;
    int i;
    
    while( bcm2835_spi_transfer( 0 ) != 0xA5 );
    bcm2835_spi_transfer( 0x5A );

    bcm2835_spi_transfernb( command, receive, size );
    for( i = 0; i < size; i++ ) {
        receive[i] = ~receive[i];
    }
    if( memcmp( command, receive + ECHO_DELAY, size - extraBytes ) == 0 ) {
        successful = 1;
    }

    if( !successful ) {
        fprintf( stderr, "ERROR: Gave up on sending command after multiple attempts.\n" );
        for( i = 0; i < size - extraBytes; i++ ) {
            printf( "Sent: %02x, Got: %02x\n", command[i], receive[i + ECHO_DELAY] );
        }
    }
    return successful;
}

void writeString( char *string ) {
    for( ; *string != '\0'; string++ ) {
        writeLcdData( ( uint8_t )*string );
    }
}

void processOutputGpoCommand( uint16_t outputData, uint16_t bitMask ) {
    uint16_t originalMasked = gpoData & ~bitMask;
    uint16_t newMasked = outputData & bitMask;
    gpoData = originalMasked | newMasked;
    sendGpoData();
}

void processSetGpoCommand( uint16_t setBits ) {
    gpoData = gpoData | setBits;
    sendGpoData();
}

void processClearGpoCommand( uint16_t clearBits ) {
    gpoData = gpoData &~ clearBits;
    sendGpoData();
}

static void writeLcdCommand( uint8_t command ) {
    processSetGpoCommand( 0x0200 );
    processOutputGpoCommand( command, 0x00FF );
    processClearGpoCommand( 0x0200 );
    if( command == 0x01 ) {
        delay( 2 );
    }
}

static void writeLcdData( uint8_t data ) {
    processSetGpoCommand( 0x0300 );
    processOutputGpoCommand( data, 0x00FF );
    processClearGpoCommand( 0x0300 );
}

static void sendGpoData( void ) {
    bcm2835_i2c_write( ( char* )&gpoData, 2 );
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

static void setupI2c( void ) {
    bcm2835_i2c_begin();
    bcm2835_i2c_setClockDivider( BCM2835_I2C_CLOCK_DIVIDER_626 );
    bcm2835_i2c_setSlaveAddress( 32 );
}

static void setupLcd( void ) {
    writeLcdCommand( 0x38 );
    writeLcdCommand( 0x06 );
    writeLcdCommand( 0x0C );
    writeLcdCommand( 0x01 );
}
