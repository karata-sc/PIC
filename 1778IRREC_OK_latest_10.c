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
uint16_t preReg = 0;
uint16_t dataReg = 0;
uint16_t nextReg = 0;
uint16_t nextDataReg = 0;
uint8_t Data[4];
int cnt = 31;
int flag = 0;
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    OSCTUNE = 0x00;
    INTCON = 0xC0; 
    T1CON = 0b01110000; //PRS 1:8 1u sec
    T1CONbits.TMR1ON = 0;
    //TMR1H = 65036 >> 8; // 0.5m sec interrupt
    //TMR1L = 65036 & 0x00FF;
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;
    
    /*INTCONbits.TMR0IE = 1;
    OPTION_REG = 0b10000000; //PRS 1:2 1u sec
    TMR0 = 156; //0.1m sec interrupt*/
    
    IOCCP = 0x00;//falling
    IOCCN = 0x01;
    IOCCF = 0;
    INTCONbits.IOCIE = 1;
    
    PORTA = 0b00000000;
    //OPTION_REGbits.nWPUEN = 0;
    //WPUC = 0xFF;
    
    while(1)
    {   
        //PORTA = nextDataReg >> 12;
        if(nextDataReg==0x58A7)
        {
            PORTA = 0x0C;
            __delay_ms(1000);
            __delay_ms(1000);
            dataReg=0;
            nextDataReg=0;
        }
        else if(nextDataReg==0x7887)
        {
            PORTA = 0x03;
            __delay_ms(1000);
            __delay_ms(1000);
            dataReg=0;
            nextDataReg=0;
        }
        else{
            PORTA = 0x00;
        }
        /*if((nextDataReg!=0) && (dataReg!=0)){
            for(int i=0;i<4;i++){    
                __delay_ms(1000);
                __delay_ms(1000);
                PORTA = ( nextDataReg & (15 << (4*i)) ) >> (4*i);
            }
            for(int i=0;i<4;i++){    
                __delay_ms(1000);
                __delay_ms(1000);
                PORTA = ( dataReg & (15 << (4*i)) ) >> (4*i);
            }
            nextDataReg = 0;
            dataReg = 0;
        }else{
            PORTA = 0x00;
        }*/
    }
}

/*----------------------------------------------------------------------*/

void __interrupt() isr(void)
{
    INTCONbits.GIE = 0;
    
    if(IOCCFbits.IOCCF0)
    {
        IOCCF = 0;
        switch(state){
            case 0:
                INTCONbits.IOCIE = 0;
                TMR1H = 59536 >> 8; // 6m sec interrupt
                TMR1L = 59536 & 0x00FF;
                T1CONbits.TMR1ON = 1;
                break;
                
            case 1:
                state = 2;
                IOCCP = 0x01;//Rising
                IOCCN = 0x00;
                break;
                
            case 2:
                INTCONbits.IOCIE = 0;
                TMR1H = 64636 >> 8; // 1.5T interrupt
                TMR1L = 64636 & 0x00FF;
                T1CONbits.TMR1ON = 1;
                break;
                
            case 3:
                INTCONbits.IOCIE = 0;
                cnt = 31;
                IOCCP = 0x00;//Falling
                IOCCN = 0x01;
                dataReg = preReg;
                nextDataReg = nextReg;
                preReg = 0;
                nextReg = 0;
                state = 0;
                INTCONbits.IOCIE = 1;
                break;
                
            default:
                break;
        }
    }
    
    else if(PIR1bits.TMR1IF)
    {
        PIR1bits.TMR1IF = 0;
        switch(state)
        {
            case 0:
                if(!IRIN)
                {
                    state = 1;
                }
                T1CONbits.TMR1ON = 0;
                INTCONbits.IOCIE = 1;
                break;
                
            case 2:
                if(IRIN){
                    if(cnt<16)
                        nextReg |= 1 << cnt; 
                    else
                        preReg |= 1 << (cnt-16); 
                }
                T1CONbits.TMR1ON = 0;
                cnt--;
                INTCONbits.IOCIE = 1;
                if(cnt==-1){
                    state = 3;
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
