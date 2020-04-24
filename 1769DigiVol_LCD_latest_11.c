/*** 1769DigiVol_LCD_latest ***/
#include "mcc_generated_files/mcc.h"

#define SET_CS PORTCbits.RC4 //LM1972 pin : LOAD/SHIFT, SS
#define SET_RS PORTCbits.RC6 //LCD pin : RS
#define SET_E  PORTCbits.RC7  //LCD pin : E
#define IRIN   PORTCbits.RC0

int cnt_bit = 31;
int state = 0;
uint8_t preReg1 = 0;
uint8_t preReg2 = 0;
uint8_t preReg3 = 0;
uint8_t preReg4 = 0;
uint8_t dataReg = 0;

int vol_lcd = 790;  //(MUTE) 
uint8_t vol = 127;  //(MUTE) 
uint8_t StMsg[]="LM1972M";
uint8_t Buffer[]="LEVEL> xxxxx dB";
uint8_t Mute[]=  "LEVEL> MUTE    ";

void SetVol(uint8_t ch, uint8_t vol_data);
void UpVol(void);
void DownVol(void);

void itostring(int digit, int data, uint8_t *buffer);
void lcd_out(uint8_t code, uint8_t flag);
void lcd_data(uint8_t asci);
void lcd_cmd(uint8_t cmd);
void lcd_clear();
void lcd_str(uint8_t *buf);
void lcd_init();

/***  main  *******************************************/
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    // initialize SPI (LM1972M)
    RC5PPS = 0x12;          // RC5:SCK
    SSPCLKPPS = 0b00010101; // SCK:RC5
    SSPSSPPS = 0b00010100;  // SS:RC4
    RA2PPS = 0x14;          // RA2:SDO
    SSP1STATbits.CKE = 0;   // idle to active
    SSP1CON1 = 0b00100000;  // Enable SerialPort / idle Low / CLK:FOSC/4
    SET_CS = 1;
    
    // initialize Interrupt
    INTCON = 0xC0;
    
    // initialize Interrupt(state_change)
    IOCAPbits.IOCAP4 = 1; // Rising
    IOCAPbits.IOCAP5 = 1;
    IOCAF = 0;
    IOCCPbits.IOCCP0 = 0;
    IOCCNbits.IOCCN0 = 1; // Falling
    IOCCF = 0;
    INTCONbits.IOCIE = 1;
    
    // initialize Interrupt(timer1)
    T1CON = 0b01110000; // PRS 1:8 1u sec
    T1CONbits.TMR1ON = 0;
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;
    
    // initialize LCD
    lcd_init();
    lcd_clear();
    lcd_str(StMsg);// line1
    
    while (1)
    {
        SetVol(1,vol);  // SPI LM1972 Ch,Vol
        
        lcd_cmd(0xC0);  // set line2
        if(vol == 127){
            lcd_str(Mute);
        }
        else{
            itostring(5, vol_lcd, Buffer+7);
            lcd_str(Buffer);   
        }
    }
}

/***  function  *******************************************/

//  Interrupt
void __interrupt() isr(void)
{    
    INTCONbits.GIE = 0;
    
    //  Interrupt(state_change En) 
    if(IOCAFbits.IOCAF4 && vol!=127)
    {
        IOCAFbits.IOCAF4 = 0;
        if(PORTAbits.RA5)
            UpVol();
    }
    else if(IOCAFbits.IOCAF5 && vol!=0)
    {
        IOCAFbits.IOCAF5 = 0;
        if(PORTAbits.RA4)
            DownVol();
    }
   
    //  Interrupt(state_change InfraRed) 
    if(IOCCFbits.IOCCF0)
    {
        IOCCFbits.IOCCF0 = 0;
        switch(state){
            case 0:
                TMR1 = 57536; // 8m sec interrupt
                T1CONbits.TMR1ON = 1;
                break;
                
            case 1:
                state = 2;
                IOCCPbits.IOCCP0 = 0;
                IOCCNbits.IOCCN0 = 1; // Falling
                TMR1 = 0;
                T1CONbits.TMR1ON = 1;
                break;
                
            case 2:
                T1CONbits.TMR1ON = 0;
                if(TMR1>1800 && TMR1<2600)//2200
                {
                    dataReg = preReg1;
                    if(dataReg==0x87 && vol!=127)
                        UpVol();
                    else if(dataReg==0xA7 && vol!=0)
                        DownVol();
                    state = 0;
                }
                else if(TMR1>4000)
                {
                    IOCCPbits.IOCCP0 = 1; // Rising
                    IOCCNbits.IOCCN0 = 0;
                    state = 3;
                }
                break;
                
            case 3:
                if(cnt_bit==31)
                {
                    preReg4 = 0;
                    preReg3 = 0;
                    preReg2 = 0;
                    preReg1 = 0;
                }
                TMR1 = 64636; // 1.5T interrupt
                T1CONbits.TMR1ON = 1;
                break;
                
            case 4:
                cnt_bit = 31;
                IOCCPbits.IOCCP0 = 0; // Falling
                IOCCNbits.IOCCN0 = 1;
                dataReg = preReg1;
                if(dataReg==0x87 && vol!=127)
                    UpVol();
                else if(dataReg==0xA7 && vol!=0)
                    DownVol();
                state = 0;
                break;
                
            default:
                break;
        }
    }
    
    else if(PIR1bits.TMR1IF)
    {
        PIR1bits.TMR1IF = 0;
        switch(state)
        {
            case 0:
                T1CONbits.TMR1ON = 0;
                if(!IRIN)
                {
                    state = 1;
                    IOCCPbits.IOCCP0 = 1; // Rising
                    IOCCNbits.IOCCN0 = 0;
                }
                break;
                
            case 3:
                if(IRIN){
                    if(cnt_bit>23)
                        preReg4 |= 1 << (cnt_bit-24);
                    else if(cnt_bit<=23 && cnt_bit>15)
                        preReg3 |= 1 << (cnt_bit-16);
                    else if(cnt_bit<=15 && cnt_bit>7)
                        preReg2 |= 1 << (cnt_bit-8);
                    else
                        preReg1 |= 1 << cnt_bit;
                }
                T1CONbits.TMR1ON = 0;
                cnt_bit--;
                if(cnt_bit==-1)
                    state = 4;
                break;
                
            default:
                break;
        }
    }
    
    INTCONbits.GIE = 1;
}

// set volume SPI
void SetVol(uint8_t ch, uint8_t vol_data)
{		
    SET_CS = 0;
    SSP1BUF = ch-1;
    while(!SSP1STATbits.BF);
    SSP1BUF = vol_data;
    while(!SSP1STATbits.BF);
    SET_CS = 1;
}

// edit volume 
void UpVol(void)
{
    vol++;
    if(vol >= 97) 
        vol_lcd+=10;
    else
        vol_lcd+=5; 
}

void DownVol(void)
{
    vol--;
    if(vol >= 96) 
        vol_lcd-=10;
    else 
        vol_lcd-=5;
}

//  set_LCD
void itostring(int digit, int data, uint8_t *buffer)
{
    int i;
    int x;
    int flag1 = 0;
    int flag2 = 0;
    int flag3 = 0;
    int flag4 = 0;
    buffer += digit;
    
    if(data==5)
    {
        flag2 = 1;
        flag3 = 1;
    }
    if(data==0)
    {
        flag2 = 1;
        flag3 = 1;
        flag4 = 1;
    }
    
    for(i=digit; i>0; i--)
    {
        buffer--;   //set
        x = data % 10;
        
        if(i==(digit-2))
            flag2 = 0;
        if(i==(digit-2) && flag3==1)
            *buffer = x + '0';
        else if(data==0 && i!=digit && flag1==1)
            *buffer = ' ';       //1%10 => 1
        else if(data==0 && i!=digit && flag1==0 && flag2==0){
            if(flag4==0)
                *buffer = '-';       //1%10 => 1
            if(flag4==1)
                *buffer = ' ';
            flag1 = 1;
        }
        else{
            if(i==(digit-1)){
                *buffer = '.';
            }else{
                *buffer = x + '0';  //1%10 => 1
                data /= 10;
            }
        }
    }
}

void lcd_out(uint8_t code, uint8_t flag)
{
    LATB = (code & 0xF0);
    if(flag == 0) 
        SET_RS=1;
    else
        SET_RS=0;
    SET_E=1;
    SET_E=0;
}

void lcd_data(uint8_t asci)
{
    lcd_out(asci,0);
    lcd_out(asci<<4,0);
    __delay_us(50);
}

void lcd_cmd(uint8_t cmd)
{
    lcd_out(cmd,1);
    lcd_out(cmd<<4, 1);
    __delay_ms(2);
}

void lcd_clear()
{
    lcd_cmd(0x01);
    __delay_ms(15);
}

void lcd_str(uint8_t *buf)
{
    while(*buf != 0)
        lcd_data(*buf++);
}

void lcd_init()
{
    __delay_ms(150);
    lcd_out(0x30,1);
    __delay_ms(5);
    lcd_out(0x30,1);
    __delay_ms(1);
    lcd_out(0x30,1);
    __delay_ms(1);
    lcd_out(0x20,1);
    __delay_ms(1);
    lcd_cmd(0x2E);
    lcd_cmd(0x08);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x02);
}
/**
 End of File
*/
