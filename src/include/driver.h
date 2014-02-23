enum GpioType {
    NORMAL = 0,
    BOOTLOAD = 1
};

typedef enum GpioType GpioType;

int gpioInit( GpioType gpioType );
int gpioClose( void );

