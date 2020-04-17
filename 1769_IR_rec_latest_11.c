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
    T1CON = 0b00010000; //PRS 1:2 // 1count+TMR1 1usec //Timer1 OFF
    TMR1H = 0;
    TMR1L = 0;
    INTCON = 0b11000000; //GIE Enable //PEIE Enable
    PIE1 = 0x00;
    TMR1IE = 1;
    PIR1 = 0x00;
    
    // initialize Interrupt (INTERRUPT-ON-CHANGE) 
    IOCIE = 1;
    IOCCP1 = 0; // Set edge type : falling  
    IOCCN1 = 1;
    IOCCF = 0;  // clear IOC flag
    
    LED1 = 0;
    
    while (1)
    {
        /*
        if(flag1){
            __delay_us(7600);
            __delay_us(4800);
            __delay_us(600);
            __delay_us(800);
            for(;cnt<4;cnt++){
                if(IRIN == 0){                
                    toReg |= 0x00;
                    __delay_us(400);
                }
                else{
                    toReg |= 0x01;
                    __delay_us(1600);
                }
            __delay_us(800);
            }
            dataChd = toReg;
        }
        */
        if(flag1){
            for(int c=0;c<5;c++){
                LED1 = 1;
                __delay_ms(100);
                LED1 = 0;
                __delay_ms(100);
            }
            flag1 = 0;
        }
        else
            LED1 = 0;
    }
}

// interrupt
void __interrupt() isr(void)
{   
    GIE = 0;
    
    if(IOCCF1){
        switch(state){
            
            case 0 :
                IOCIE = 0;      // Disable IOCI
                SetTimer1(500); // 0.5ms 
                ON_Timer1(); 
                IOCCF = 0;
                break;
                
            case 1 :
                IOCIE = 0;      // Disable IOCI
                SetTimer1(500); // 0.5ms 
                ON_Timer1(); 
                
            default :
                break;
        }
    
    if(TMR1IF){
        switch(state){
            
            case 0 :
                TMR1IF = 0; // Clear flag
                OFF_Timer1();
                if(IRIN == 0){        
                    state = 1;  
                    
                }
            }
            IOCIE = 1; // Enable IOCI 
            
            default :
                break;
    }
    
    /*
    if(IOCCF0){                 // default Set edge type : falling 
        switch(state){          // default state : 0 (chPulse)
            
            case 0 :      // Check Pulse (Noise or LeaderChord)  
                IOCIE = 0;      // Disable IOCI
                SetTimer1(500); // 0.5ms 
                ON_Timer1(); 
                break;          // To chPulse (Timer1)
                
            case 1 :      // at first bit low  // MSB comes 
                IOCIE = 0;      // Disable IOCI 	
                state = 2;
                SetTimer1(600); // 0.6mS (ignore first bit low )
                ON_Timer1();
                break;          // To " igFstBit "
            
            case 3 :              // Check MSB (High or Low)
                IOCIE = 0;           // Disable IOCI
                if(IRIN == 0){       // wait new bit  // PreBit : 1
                    SetTimer1(1400); // 1.4mS
                    ON_Timer1();
                }
                else{                // PreBit : 0 
                    SetTimer1(800);  // 0.8mS
                    ON_Timer1();
                }
                break;  // To chHL (Timer1)
                
            default:
                break;
        }
    }
    IOCCF = 0; //clear IOCIflag
    */
    
    // Timer1
    
    /*
    if(TMR1IF){
        switch(state){
            
            case 0 :  //after 0.5mS // Check Pulse (Noise or LeaderChord)  
                TMR1IF = 0; // Clear flag
                OFF_Timer1();
                
                if(IRIN == 0)        // (It's LeaderChord.)
                {
                    state = 1;
                    SetTimer1(9000); // 9mS 
                    ON_Timer1();
                    //To " bitWait " (Timer1)
                }  
                else           // (It's Noise.)  
                    IOCIE = 1; // Enable IOCI 
                    // To top.
                break;
                
            case 1 :  //after 9mS // Wait until MSB comes
                TMR1IF = 0; // Clear flag
                OFF_Timer1();
                IOCIE = 1;  // IOCI Enable
                break;      //To " bitWait " (IOCI)
                
            case 2 :     //after 0.6mS // ignore 1T (MSB)
                TMR1IF = 0;     // Clear flag
                state = 3; 
                SetTimer1(800); // 0.8mS
                ON_Timer1();
                break;          // To " chHL " (Timer1)
                
            case 3:          // Check MSB (High or Low)
                TMR1IF = 0;     // Clear flag
                OFF_Timer1();               
                
                if(IRIN == 0){  // MSB : 0
                    oneBit = 0x0;
                    toReg |= oneBit << cnt;
                    SetEdge(1); // Rising 
                }
                else{           // MSB : 1
                    oneBit = 0x1;
                    toReg |= oneBit << cnt;
                    SetEdge(0); // Falling 
                }
                cnt++;
                IOCIE = 1;      // Enable IOCI
                if(cnt==4){
                    IOCIE = 0;       // Disable IOCI 
                    state = 5;
                    SetTimer1(1000); // 1mS
                    ON_Timer1();
                }
                break;
                
            case 5 : // Check stopBit
                TMR1IF = 0;      // Clear flag
                OFF_Timer1();
                state = 0; // Check Pulse
                dataChd = toReg; // set Data // vol++ if etc...
                SetEdge(0);      // Falling 
                IOCIE = 1;       // Enable IOCI
                break;           // To top
                
            default:
                break;
        }
    }
    */
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

/**
 End of File
*/
