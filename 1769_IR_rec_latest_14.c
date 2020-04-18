/**** 1769_IR_rec_latest ****/
#include "mcc_generated_files/mcc.h"

#define IR_REC RC3
#define LED1 RC0
#define LED2 RC1

int swflag = 0;

int cnt = 0;        // counter
int oneBit = 0;     // Stor 1 bit (assign to toReg)
int toReg = 0x0;    // Stor 4 bit (assign to dataChd)
int dataChd = 0;    // Stor 4 bit

int state = 0;

// declare function
void SetEdge(int edge);

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    // initialize Interrupt (Timer1,Timer0) 
    T1CON = 0b00110001;  //PRS 1:8 // TMR1++ 4usec //Timer1 ON
    TMR1H = 0;
    TMR1L = 0;
    INTCON = 0b11000000; //GIE Clear //PEIE Clear //TMR0IE Clear
    PIE1 = 0x00;         //TMR1IE Clear
    PIR1 = 0x00;
    OPTION_REG = 0b10000010; //PRS 1:8
    TMR0 = 231;          // Interrupt interval 0.1ms

    LED1 = 0;
    LED2 = 0;

    while (1)
    {   
        switch(dataChd){
            case 1:
                for(int x=0;x<4;x++){
                    LED1 = 1;
                    __delay_ms(100);
                    LED1 = 0;
                    __delay_ms(100);
                }
                swflag = 0;
                dataChd = 0;
                TMR0 = 231; // Interrupt interval 0.1ms
                TMR0IE = 1;
                break;
               
            case 2:
                for(int y=0;y<4;y++){
                    LED2 = 1;
                    __delay_ms(100);
                    LED2 = 0;       
                    __delay_ms(100);
                }
                swflag = 0;
                dataChd = 0;
                TMR0 = 231; // Interrupt interval 0.1ms
                TMR0IE = 1;
                break;
                
            default:
                LED1 = 0;
                LED2 = 0;
                break;
        }
    }
}

/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
// interrupt
void __interrupt() isr(void)
{   
    GIE = 0;
    
    if(TMR0IF){
        TMR0IF = 0;
        if(IR_REC == 0){
            TMR0IE = 0;
            TMR1H = (65536 - 250) >> 8; // 1m sec (noise check)
            TMR1L = (65536 - 250) & 0x00FF;
            TMR1IE = 1;
        }
        else    
            TMR0 = 231;
    }
    
    /*-----------------------------------------------------------------*/
    
    if(TMR1IF){
        TMR1IF = 0;
        switch(state){
            
            case 0:
                if(IR_REC == 0){
                    state = 1;
                    TMR1H = (65536 - 5300) >> 8; // (40 + 40 + 10T) - (1msec)
                    TMR1L = (65536 - 5300) & 0x00FF;            
                }else{
                    TMR1IE = 1;
                    TMR0 = 231; //0.1m sec
                    TMR0IE = 1;
                }
                break;
                
            case 1:
                if(IR_REC == 0){
                    // bit : 0
                    TMR1H = (65536 - 4500) >> 8; 
                    TMR1L = (65536 - 4500) & 0x00FF;            
                }else{
                    // bit : 1
                    swflag++;
                    TMR1H = (65536 - 16500) >> 8; 
                    TMR1L = (65536 - 16500) & 0x00FF;            
                }
                state = 2;
                break;
                
            case 2:  
                TMR1H = (65536 - 7500) >> 8; 
                TMR1L = (65536 - 7500) & 0x00FF;  
                state = 3;
                break;
                
                
                if(IR_REC){
                    swflag++;     
                }
                state = 3;  
                TMR1H = (65536 - 12000) >> 8; 
                TMR1L = (65536 - 12000) & 0x00FF; 
                break;
                
            case 3:
                 if(IR_REC){
                    swflag++;     
                }
                state = 4;  
                TMR1H = (65536 - 12000) >> 8; 
                TMR1L = (65536 - 12000) & 0x00FF; 
                break;
                
            case 4:
                TMR1IE = 0;
                dataChd = swflag;
                break;

            default:
                break;
        }
    }
    GIE = 1;
}
/*
 END OF FILE
 */
