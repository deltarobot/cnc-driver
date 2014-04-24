extern int gpioInit( void );
extern int gpioClose( void );
extern int resetController( void );
extern int processMotorCommand( char *send, char *receive, int numberCommands, int size, int extraBytes );
extern void writeString( char *string );
extern void processOutputGpoCommand( uint16_t outputData, uint16_t bitMask );
extern void processSetGpoCommand( uint16_t setBits );
extern void processClearGpoCommand( uint16_t clearBits );
