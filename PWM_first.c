/** PWM_OUT_latest **/
#include "mcc_generated_files/mcc.h"

#define LED1 RC5
#define LED2 RC4
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    //1
    RC5PPS = 0xC;//CCP1
    TRISC5 = 1;
    
    //2
    PR2 = 52;
    
    //3,4
    CCPR1L = 68 >> 2; //8bit
    CCP1CON = 0x0C;
    //CCP1CON = 0x0C | ((68 & 0x03)<<4); //2bit
    DC1B0 = 68 & 0x01;
    DC1B1 = 68 & 0x02;
    
    //5
    TMR2IF = 0; //PIR1
    //T2CON = 0b00000100;
    T2CKPS0 = 0;
    T2CKPS1 = 0;
    TMR2ON = 1;
    
     //6
    while(TMR2IF==0);
    TRISC5 = 0;
     
    /*
    PWM3CON = 0;

    PWM3DCH = 68 >> 2;
    PWM3DCL = 68 & 0x003;
    TMR2IF = 0;
    T2CON = 0b0000100;
    */
     
    while (1)
    {
        ;//LED1 = 1;
    }
}
/**
 End of File
*/

