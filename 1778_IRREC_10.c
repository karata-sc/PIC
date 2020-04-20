/**** 1778_IRREC ****/
#include "mcc_generated_files/mcc.h"
#include <stdio.h>
#define LED0 PORTAbits.RA0
#define LED1 PORTAbits.RA1
#define LED2 PORTAbits.RA2
#define LED3 PORTAbits.RA3
#define IRIN PORTCbits.RC0
#define LEDCH PORTBbits.RB5
int state = 0;
int preReg = 0x0;
int dataReg = 0x0;

int cnt = 3;
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    INTCON = 0xC0;
    
    T1CON = 0b00010000; //PSR 1:2 1u sec
    T1CONbits.TMR1ON = 0;
    //TMR1H = 65036 >> 8; // 0.5m sec
    //TMR1L = 65036 & 0x00FF;
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;
    
    IOCCNbits.IOCCN0 = 1;
    IOCCF = 0;
    INTCONbits.IOCIE = 1;
    PORTA = 0x00;
    //OPTION_REGbits.nWPUEN = 0;
    //WPUC = 0xFF;
    while (1)
    {
        PORTA = dataReg;
    }
}

/*----------------------------------------------------------------------*/

void __interrupt() isr(void){
    INTCONbits.GIE = 0;
    if(INTCONbits.IOCIF){ //IOCCF0
        IOCCF = 0; //NOT IOCCFx
        INTCONbits.IOCIE = 0;
        TMR1H = 60536 >> 8; // 5.0m sec
        TMR1L = 60536 & 0x00FF;
        T1CONbits.TMR1ON = 1;
    }
    else if(PIR1bits.TMR1IF){
        PIR1bits.TMR1IF = 0;
        switch(state){
            case 0:
                if(!IRIN){
                    state = 1;
                    dataReg = 0x0;
                    TMR1H = 55536 >> 8; // 10.0m sec
                    TMR1L = 55536 & 0x00FF;
                }
                else{
                    T1CONbits.TMR1ON = 0;
                    INTCONbits.IOCIE = 1;
                }
                break;
                
            case 1:
                if(!IRIN){
                    preReg |= 0x1 << cnt;
                }
                cnt--;
                TMR1H = 55536 >> 8; // 1.0m sec
                TMR1L = 55536 & 0x00FF;
                if(cnt<0){
                    T1CONbits.TMR1ON = 0;
                    cnt = 3;
                    state = 0;
                    dataReg = preReg;
                    preReg = 0x0;
                    INTCONbits.IOCIE = 1;
                }
                break;
                
            default:
                break;
        }
    }
    INTCONbits.GIE = 1;
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
