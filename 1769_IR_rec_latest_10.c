/**** 1769_IR_rec_latest ****/
/*    2020/4/16/Tue/21:21   */
/****************************/
#include "mcc_generated_files/mcc.h"

#define IRIN RC0
#define LED1 RC3

int flag1 = 0;
int flag2 = 0;
int flag3 = 0;

int cnt = 0;        // counter
int oneBit = 0;     // Stor 1 bit (assign to toReg)
int toReg = 0x0;    // Stor 4 bit (assign to dataChd)
int dataChd = 0x0;  // Stor 4 bit

int state = 0;
// name state
int chNoise = 0;    // Check Pulse (Noise or LeaderChord)  
int bitWait = 1;    // Wait until MSB comes
int igFstBit = 2;   // ignore 1T (MSB)
int chHL = 3;       // Check MSB (High or Low)
int chHL2t0 = 4;    // Check Bit <2:0> (High or Low)
int endCh = 5;      // Check stopBit

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
    IOCCP7 = 0; // Set edge type : falling  
    IOCCN7 = 1;
    IOCCF = 0;  // clear IOC flag
    
    LED1 = 0;
    
    while (1)
    {
        if(dataChd == 0xE)
            LED1 = 1;
        else
            LED1 = 0;
    }
}

// interrupt
void __interrupt() isr(void)
{   
    GIE = 0;
    if(IOCCF0){        // default Set edge type : falling 
        switch(state){ // default state : 0 (chNoise)
            
            case chNoise :  // Check Pulse (Noise or LeaderChord)  
                // IOC disable
                IOCIE = 0;  	
                //set timer1 0.5mS 
                TMR1H = 65036 >> 8; 
                TMR1L = 65036 & 0x00FF;
                // Timer1 ON
                T1CON |= 0x01;  //main? not necessary?
                break;
                
            case bitWait :  // at first bit low// Wait until MSB comes
                state = igFstBit;
                // IOCI disable
                IOCIE = 0;  	
                // set timer1 0.6mS (ignore first bit low )
                TMR1H = 64936 >> 8; 
                TMR1L = 64936 & 0x00FF;
                // Timer1 ON
                T1CON |= 0x01;//main?
                break;
            
            case chHL : // Check MSB (High or Low)
                // IOCI disable
                IOCIE = 0;  	                
                if(IRIN == 0){  //wait new bit
                    //state = 8;//flag? 
                    // Reset flag
                    TMR1IF = 0; 
                    //Reset timer1 0.8mS
                    TMR1H = 64736 >> 8;
                    TMR1L = 64736 & 0x00FF;
                    // Timer1 ON
                    T1CON |= 0x01;
                }
                else{
                    //state = 9;
                    // set timer1 1.4mS
                    // Reset flag
                    TMR1IF = 0; 
                    //Reset timer1 0.8mS
                    TMR1H = 64136 >> 8;
                    TMR1L = 64136 & 0x00FF;
                    // Timer1 ON
                    T1CON |= 0x01;
                }
                // Timer1 ON
                //T1CON |= 0x01;//main? not necessary?
                break;
                
            default:
                break;
        }
    }
    IOCCF=0; //clear flag
    
 ///////////////////////////////////////////////////////////////////// timer
    
    if(TMR1IF){
        switch(state){
            
            case chNoise : // Check Pulse (Noise or LeaderChord)  
                // Timer1 OFF
                T1CON &= 0x10;
                // Reset flag
                TMR1IF = 0; 
                //Reset timer1 0.5mS
                TMR1H = 65036 >> 8;
                TMR1L = 65036 & 0x00FF;
                if(IRIN == 0)
                {
                    state = bitWait;
                    //set timer1 9mS 
                    TMR1H = 56536 >> 8; 
                    TMR1L = 56536 & 0x00FF;
                    // Timer1 ON
                    T1CON |= 0x01;//main? not necessary?
                }
                else    // (Noise. to top)
                {   
                    // IOCI Enable
                    IOCIE = 1;
                }
                break;
                
            case bitWait : //after 8mS // Wait until MSB comes
                // Reset flag
                TMR1IF = 0; 
                 // Timer1 OFF
                T1CON &= 0x10;
                // IOCI Enable
                IOCIE = 1;  
                break;
                
            case igFstBit : //after 0.6mS // ignore 1T (MSB)
                // Reset flag
                TMR1IF = 0; 
                //Reset timer1 0.8mS
                TMR1H = 64736 >> 8;
                TMR1L = 64736 & 0x00FF;
                // Timer1 ON
                T1CON |= 0x01;
                state = chHL; //3
                break;
                
            case chHL: // Check MSB (High or Low)
                // Reset flag
                TMR1IF = 0; 
                //Reset timer1 0.6mS
                TMR1H = 64936 >> 8;
                TMR1L = 64936 & 0x00FF;
                // Timer1 OFF
                T1CON &= 0x10;
                
                if(IRIN == 0){ // LOW
                    //1 : bit 0
                    oneBit = 0x0;
                    toReg = 0xF & (oneBit << cnt);
                    // Set edge type : rising 
                    IOCCP = 1;
                    IOCCN = 0;
                    IOCIE = 1;
                }
                else{
                    //1 : bit 1
                    oneBit = 0x1;
                    toReg = 0xF & (oneBit << cnt);
                    // Set edge type : falling 
                    IOCCP = 0;
                    IOCCN = 1;
                    IOCIE = 1;
                }
                cnt++;
                break;
                
            case chHL2t0 : // Check Bit <2:0> (High or Low)
                // Reset flag
                TMR1IF = 0; 
                //Reset timer1 0.6mS
                TMR1H = 64936 >> 8;
                TMR1L = 64936 & 0x00FF;
                // Timer1 OFF
                T1CON &= 0x10;
                
                if(IRIN == 0){ // LOW
                    //1 : bit 0
                    oneBit = 0x0;
                    toReg = 0xF & (oneBit << cnt);
                    // Set edge type : rising 
                    if(cnt<3){
                        IOCCP = 1;
                        IOCCN = 0;
                        IOCIE = 1;
                    }
                }
                else{
                    //1 : bit 1
                    oneBit = 0x1;
                    toReg = 0xF & (oneBit << cnt);
                    // Set edge type : falling 
                    if(cnt<3){
                        IOCCP = 0;
                        IOCCN = 1;
                        IOCIE = 1;
                    }
                }
                cnt++;
                if(cnt==3){
                    // IOCI disable
                    IOCIE = 0;
                    state = endCh;
                    //set timer1 1mS 
                    TMR1H = 64536 >> 8; 
                    TMR1L = 64536 & 0x00FF;
                    // Timer1 ON
                    T1CON |= 0x01;//main? not necessary?
                }
                break;
                
            case endCh : // Check stopBit
                state = chNoise;
                // Reset flag
                TMR1IF = 0; 
                //Reset timer1 0.6mS
                TMR1H = 64936 >> 8;
                TMR1L = 64936 & 0x00FF;
                // Timer1 OFF      
                T1CON &= 0x10;
                // Set edge type : falling 
                IOCCP = 0;
                IOCCN = 1;
                IOCIE = 1;
                // set Data
                dataChd = toReg;
                break;
                
            default:
                break;
        }
    }
    GIE = 1;
}
/**
 End of File
*/
