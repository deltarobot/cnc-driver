extern int gpioInit( void );
extern int gpioClose( void );
extern int resetController( void );
extern int processMotorCommand( char *send, char *receive, int size, int extraBytes );

