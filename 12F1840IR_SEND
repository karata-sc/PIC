/******** 12F1840IR_SEND *** *****/
#include "mcc_generated_files/mcc.h"
#define SW1 PORTAbits.RA4
#define SW2 PORTAbits.RA5
#define IRLED PORTAbits.RA2
int flag = 0;
void T1High(int times);
void T1Low(int times);
//void T1High(void);
//void T1Low(void);
void Bit1(void);
void Bit0(void);
void main(void){
    // initialize the device
    SYSTEM_Initialize();   
    IOCANbits.IOCAN4 = 0;
    IOCANbits.IOCAN5 = 0;
    IOCAPbits.IOCAP4 = 1;
    IOCAPbits.IOCAP5 = 1;
    IOCAF = 0;
    INTCON = 0xC8;
    IRLED = 0;
    while(1)
    {
        switch(flag){
            
            case 1 : // 1001 0x09
                T1High(15);
                T1Low(7);
                /*-------1------*/
                Bit0();
                Bit0();
                Bit0();
                Bit0();
                
                Bit0();
                Bit0();
                Bit1();
                Bit0();
                /*------2------*/
                Bit1();
                Bit1();
                Bit1();
                Bit1();
                
                Bit1();
                Bit1();
                Bit0();
                Bit1();
                /*******3*****/
                Bit0();
                Bit1();
                Bit0();
                Bit1();
                
                Bit1();
                Bit0();
                Bit0();
                Bit0();
                /*********4******/
                Bit1();
                Bit0();
                Bit1();
                Bit0();
                
                Bit0();
                Bit1();
                Bit1();
                Bit1();
                
                T1High(1);
                
                flag = 0;
                break;
            
            case 2 : // 1101 
                T1High(15);
                T1Low(7);
                
                Bit1();
                Bit1();
                Bit0();
                Bit1();
                
                Bit0();
                Bit1();
                Bit1();
                Bit1();
                
                T1High(1);
                
                flag = 0;
                break;
                
            default : 
                IRLED = 0;
                break;
        }
    }
}
void __interrupt() isr(void)
{
    INTCONbits.GIE = 0;
    if(IOCAFbits.IOCAF4){
        IOCAF = 0;
        //__delay_us(100);
        if(PORTAbits.RA4)
            flag = 1;
    }
    else if(IOCAFbits.IOCAF5){
        IOCAF = 0;
        //__delay_us(100);
        if(PORTAbits.RA5)
        flag = 2;
    }
    INTCONbits.GIE = 0;
}

void T1High(int times){
    for(int j=0; j<times; j++){
        for(int i=0; i<16; i++){ //23
            IRLED = 1;
            __delay_us(8);
            IRLED = 0;
            __delay_us(18);
        }        
    }
}

void T1Low(int times){
    for(int j=0; j<times; j++){ //23
        for(int i=0; i<16; i++){
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

/*void ByteReg(uint8_t bitReg){
    
}*/
/**
 End of File
*/
