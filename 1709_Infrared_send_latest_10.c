/******** 1709_Infrared_send_latest *** *****/
#include "mcc_generated_files/mcc.h"

#define IR1 RC5
#define SW1 RB7
#define LED1 RC4

void T1Low(int times);
void T1High(int times);
void Bit1(void);
void Bit0(void);

int swflag = 0;

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    IOCBP7 = 1; 
    IOCBF = 0;
    INTCON = 0x88;
    
    int setBit = 0;
    int dataChd = 0xE;//0b0101;
    
    while (1)
    {
        if(swflag){
            T1High(100);
            /*
            // leader chord
            T1High(16);
            T1Low(8);
            // Data chord
            for(int i=3,conv_bit=0x8; i>=0; i--,conv_bit>>=1){
                 setBit = (dataChd & conv_bit) >> i;
                 if(setBit) 
                     Bit1();
                 else
                     Bit0();
            }
            // Stop Bit
            T1High(1);
            T1Low(5);
            // flag clear
            */
            swflag = 0;    
        }
        else
            IR1 = 0;
    }
}

// Interrupt(state_change)
void __interrupt() isr(void)
{
    if(IOCBF7){
        swflag = 1;
    }
    IOCBF=0; //clear flag
}

void T1High(int times)
{
    for(int j=0; j<times; j++){
        for(int i=0; i<23; i++){
            IR1 = 1;
            __delay_us(8);
            IR1 = 0;
            __delay_us(18);
        }        
    }
}

void T1Low(int times){
    for(int j=0; j<times; j++){
        for(int i=0; i<23; i++){
            __delay_us(26);
        }
    }
}

void Bit1(void){
    T1High(1);
    T1Low(3);
}

void Bit0(void){
    T1High(1);
    T1Low(1);
}

/**
 End of File
*/
