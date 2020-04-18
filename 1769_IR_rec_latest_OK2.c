/**** 1769_IR_rec_latest ****/
#include "mcc_generated_files/mcc.h"

#define IR_REC RC3
#define LED1 RC0
#define LED2 RC1

int flag1 = 0;
int flag2 = 0;
int flag3 = 0;

int swflag = 0;

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
    PIE1 = 0b000000000;
    TMR1IE = 0;
    PIR1 = 0b00000000;
    
    // initialize Interrupt (Timer0)
    OPTION_REG = 0b10000010;
	TMR0 = 231;
    TMR0IE = 1;
    // initialize Interrupt (INTERRUPT-ON-CHANGE) 
    
    IOCIE = 1;
    IOCCP3 = 0; // Set edge type : falling  
    IOCCN3 = 1;
    IOCCF = 0;  // clear IOC flag
    
    
    LED1 = 0;
    LED2 = 0;
    
    TMR1H = (65536 - 250) >> 8;
    TMR1L = (65536 - 250) & 0x00FF;
    TMR1IE = 0;
    ON_Timer1(); 
    
    
    while (1)
    {   
        if(swflag == 1){
            for(int x=0;x<4;x++){
                LED1 = 1;
                __delay_ms(100);
                LED1 = 0;
                __delay_ms(100);
            }
            swflag = 0;
            TMR0IE = 1;
        }else if(swflag == 2){
            for(int y=0;y<4;y++){
                LED2 = 1;
                __delay_ms(100);
                LED2 = 0;
                __delay_ms(100);
            }
            swflag = 0;
            TMR0IE = 1;
        }else{
            LED1 = 0;
            LED2 = 0;
        }
    }
}

// interrupt
void __interrupt() isr(void)
{   
    GIE = 0;
    
    if(TMR0IF){
        TMR0IF = 0;
		TMR0 = 231;
        if(IR_REC == 0){
            TMR0IE = 0;
            TMR1H = (65536 - 250) >> 8;
            TMR1L = (65536 - 250) & 0x00FF;
            TMR1IE = 1;
        }
    }
    /*
    if(IOCCF3){
        IOCCF3 = 0;
        IOCIE = 0;
        TMR1H = (65536 - 250) >> 8;
        TMR1L = (65536 - 250) & 0x00FF;
        TMR1IE = 1;
        //ON_Timer1(); //1000u sec
    }
    */
    if(TMR1IF){
        TMR1IF = 0;
        TMR1IE = 0;
        //OFF_Timer1();
        switch(state){
            case 0:
                if(IR_REC == 0){
                    state = 1;
                    TMR1H = (65536 - 35000) >> 8;
                    TMR1L = (65536 - 35000) & 0x00FF;            
                    TMR1IE = 1;
                    //ON_Timer1();
                }else{
                    TMR0IE = 1;
                }
                break;
                
            case 1:
                state = 0;
                if(IR_REC == 0){
                    swflag = 1;
                }else{
                    swflag = 2;
                }
                break;
                
            default :
                ;
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
