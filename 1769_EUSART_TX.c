/*** 1769_UART ***/ 
#include "mcc_generated_files/mcc.h"
#include <stdio.h>
void TX_Char(uint8_t txData);
void putch(char Data);

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    //data sheet : P436
    
    // initialize EUSART
    RC1STAbits.SPEN = 1;
    BAUD1CONbits.BRG16 = 1; 
    SP1BRG = 207; //baud rate : 9600
    TX1STAbits.TXEN = 1;
    TX1STAbits.BRGH = 1;
    TX1REG = 0x00;
    RC1PPS = 0x16; // TX/CK

    while(1)
    {
        printf("\r\nHELLO");
        __delay_ms(1000);
    }
}

void TX_Char(uint8_t txData)
{
    while(!PIR1bits.TXIF); // 前の文字の送信完了を待つ（フラグのセットを待つ）
    TX1REG = txData;    
}

void putch(char Data)
{
    TX_Char(Data);
}
/**
 End of File
*/
