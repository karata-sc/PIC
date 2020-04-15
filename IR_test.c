/*** 1709_Infrared_send_Me ***/

#include "mcc_generated_files/mcc.h"

#define IR1 RC5
#define SW1 RB7

void ON600u(int times);
void OFF600u(int times);

int SWflag = 0;

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    INTCON = 0b11000000;
    T1CON = 0b00110001;
    //PIE1 = 0b00000001;
    TMR1IE = 1;
    //PIR1 = 0b00000000;
    TMR1IF = 0;
    
    TMR1H = 65511 >> 8;
    TMR1L = 65511 & 0x00FF;
    
    IR1 = 0;
    while (1)
    {
        if(SWflag == 1)
            ON600u(1);
        else
            OFF600u(1);
    }
}

void __interrupt() isr(void)
{   
    GIE = 0;    
    if(TMR1IF){
        TMR1IF = 0;
        TMR1H = 40536 >> 8;
        TMR1L = 40536 & 0x00FF;
        if(SW1==1){
            SWflag = 1;
        }
        else{
            SWflag = 0;
        }
    }
    GIE = 1;   
}

void ON600u(int times)
{
    for(int j=0; j<times; j++){
        for(int i=0; i<21; i++){
            IR1 = 1;
            __delay_us(8);
            IR1 = 0;
            __delay_us(18);
        }        
    }
}

void OFF600u(int times){
    for(int j=0; j<times; j++){
        for(int i=0; i<21; i++){
            __delay_us(26);
        }
    }
}

/**
 End of File
*/


    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
