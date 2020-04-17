/**** 1769_IR_rec_latest ****/
#include "mcc_generated_files/mcc.h"

#define IRIN RC1
#define LED1 RC0
#define LED2 RC2

int flag1 = 0;
int flag2 = 0;
int flag3 = 0;

int cnt = 0;        // counter
int oneBit = 0;     // Stor 1 bit (assign to toReg)
int toReg = 0x0;    // Stor 4 bit (assign to dataChd)
int dataChd = 0x0;  // Stor 4 bit

int state = 0;
// name state
int chPulse = 0;    // Check Pulse (Noise or LeaderChord)  
int bitWait = 1;    // Wait until MSB comes
int igFstBit = 2;   // ignore 1T (MSB)
int chHL = 3;       // Check MSB (High or Low)
int chHL2t0 = 4;    // Check Bit <2:0> (High or Low)
int endCh = 5;      // Check stopBit

// declare functions 
void SetTimer1(int time);
void ON_Timer1(void);
void OFF_Timer1(void);
void SetEdge(int edge);


void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    // initialize Interrupt (Timer1) 
    T1CON = 0b00110000; //PRS 1:8 // 1count+TMR1 1usec //Timer1 OFF
    TMR1H = 0;
    TMR1L = 0;
    INTCON = 0b11000000; //GIE Enable //PEIE Enable
    PIE1 = 0x00;
    TMR1IE = 0;
    PIR1 = 0x00;
    
    // initialize Interrupt (INTERRUPT-ON-CHANGE) 
    IOCIE = 1;
    IOCCP1 = 0; // Set edge type : falling  
    IOCCN1 = 1;
    IOCCF = 0;  // clear IOC flag
    
    LED1 = 0;
    
    while (1)
    {
        switch(flag1){
            case 1 :
                LED1 = 1;
                __delay_ms(1000);
                flag1 = 0;
                break;
            case 2 :
                 LED2 = 1;
                __delay_ms(1000);
                flag1 = 0;     
                break;
            default :
                LED1 = 0;
                LED2 = 0;
                break;
        }
    }
}

// interrupt
void __interrupt() isr(void)
{   
    GIE = 0;
    
    if(IOCCF1){
        if(state==0){
                IOCCF1 = 0;
                IOCIE = 0;      // Disable IOCI
                TMR1H = (65536 - 6000) >> 8;
                TMR1L = (65536 - 6000) & 0x00FF;
                TMR1IE = 1;
                ON_Timer1(); 
        }
    }
    
    if(TMR1IF){
        switch(state){
            
            case 0 :
                TMR1IF = 0; // Clear flag
                OFF_Timer1();
                TMR1IE = 0;
                if(IRIN == 0){        
                    state = 1;  
                    TMR1H = (65536 - 22000) >> 8;
                    TMR1L = (65536 - 22000) & 0x00FF;
                    TMR1IE = 1;
                    ON_Timer1(); 
                }
                else{
                    IOCIE = 1; // Enable IOCI 
                }
                break;
            
            case 1 :
                TMR1IF = 0; // Clear flag
                OFF_Timer1();
                TMR1IE = 0;
                state = 2;
                if(IRIN == 0){
                    flag1 = 1;
                    TMR1H = (65536 - 1) >> 8;
                    TMR1L = (65536 - 1) & 0x00FF;
                    TMR1IE = 1;
                    ON_Timer1(); 
                }
                else{
                    flag1 = 2;
                    TMR1H = (65536 - 1) >> 8;
                    TMR1L = (65536 - 1) & 0x00FF;
                    TMR1IE = 1;
                    ON_Timer1(); 
                }
                break;
                
            case 2 :
                TMR1IF = 0; // Clear flag
                OFF_Timer1();
                TMR1IE = 0;
                state = 0;
                IOCIE = 1; // Enable IOCI 
                break;
                
            default :
                break;
        }
    }
    GIE = 1;
}

//  function
void SetTimer1(int time){   // uS
    TMR1H = (65536 - time) >> 8;
    TMR1L = (65536 - time) & 0x00FF;
}

void ON_Timer1(void){
    // Timer1 ON
    T1CON |= 0x01; 
} 

void OFF_Timer1(void){
    // Timer1 OFF
    T1CON &= 0x10;
}

void SetEdge(int edge){
    if(edge){
        // Set edge type : rising 
        IOCCP1 = 1;
        IOCCN1 = 0;
    }
    else{
        // Set edge type : falling 
        IOCCP1 = 0;
        IOCCN1 = 1;   
    }
}
