//  timer1_interrupt
#include "mcc_generated_files/mcc.h"

#define LED RC2
int cnt = 0;
int flag=0;
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    T1CON = 0b00110001;
    TMR1H = 40536 >> 8;
    TMR1L = 40536 & 0x00FF;
    INTCON = 0b11000000;
    PIE1 = 0b00000001;
    PIR1 = 0b00000000;
    
    while(1) LED = flag;
}

//  Interrupt(timer0) 
void __interrupt() isr(void)
{   
    GIE = 0;
    if(TMR1IF){
        TMR1IF = 0;
        TMR1H = 40536 >> 8;
        TMR1L = 40536 & 0x00FF;
        cnt++;
        if(cnt == 10){
            flag = ~flag;
            cnt = 0;
        }
    }
    GIE = 1;
}

/**
 End of File
*/
