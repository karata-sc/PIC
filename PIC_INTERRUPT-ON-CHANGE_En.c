#include "mcc_generated_files/mcc.h"
// FOSC = INTOSC(8MHz)

int cnt = 0;
/*** main *******************************************/
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    //initialize Interrupt(state_change)
    IOCAP4 = 1;
    IOCAP5 = 1;
    IOCAF = 0;
    INTCON = 0x88;

    while(1);
}

// Interrupt(state_change)
void __interrupt() isr(void)
{
    if(IOCAF4){
        __delay_ms(1);
        if(RA5) cnt++;
    }
    if(IOCAF5 && cnt!=0){
        __delay_ms(1);
        if(RA4) cnt--;
    }
    IOCAF=0; //clear flag
}
/**
End of File
*/
