#include "mcc_generated_files/mcc.h"


#define SW1 RB7
#define LED1 RB4
#define IR_LED1 RA5

int flag = 0;

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    IOCBP7 = 1;
    IOCBF7 = 0;
    INTCON = 0x88;
    
    LED1 = 0;
    IR_LED1 = 0;
    SW1 = 0;
    
    while (1)
    {
        LED1 = flag;
    }
}

void __interrupt() isr(void)
{
    if(IOCBF){
        LED1 = ~LED1;
    }
    IOCBF = 0;
}

/**
 End of File
*/
