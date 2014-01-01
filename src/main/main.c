#include <stdio.h>
#include "driver.h"

int main( void ) {
    int i;

    printf( "Hello, World\n" );

    for( i = 0; i < TEST; ++i ) {
        printf( "%d\n", i );
    }

    return 0;
}