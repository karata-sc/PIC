/*** 1769DigiVol_LCD_latest ***/
#include "mcc_generated_files/mcc.h"

#define SET_CS RC4// LOAD/SHIFT
#define SDO RA2
#define SCK RC5
#define SET_RS RC6
#define SET_E RC7
#define IRIN RC0

int state = 0;
uint8_t preReg1 = 0;
uint8_t preReg2 = 0;
uint8_t preReg3 = 0;
uint8_t preReg4 = 0;
uint8_t dataReg = 0;
int cnt = 31;

int vol = 127;      //(MUTE) from PFM
int vol_lcd = 790;  //(MUTE) from PFM
unsigned char StMsg[]="LM1972M";
unsigned char Buffer[]="LEVEL> xxxxx dB";
unsigned char Mute[]=  "LEVEL> MUTE    ";

void setVol(int ch, int vol_data);
void itostring(int digit, int data, unsigned char *buffer);
void lcd_out(unsigned char code, unsigned char flag);
void lcd_data(unsigned char asci);
void lcd_cmd(unsigned char cmd);
void lcd_clear();
void lcd_str(unsigned char *buf);
void lcd_init();

/***  main  *******************************************/
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    // SPI 
    /*RA2PPS = 0x14; //SDO
    RC5PPS = 0x12; //SCK
    SSPSSPPS = 0b00010100;  //RC4
    SSPCLKPPS = 0b00010101; //RC5
    SSPDATPPS = 0b00000010; //RA2
    SSP1STATbits.CKE = 0;   //idle to active
    SSP1CON = 0b00100000;   //Enable SerialPort / idle Low / CLK:FOSC/4
            */
    //initialize Interrupt
    INTCON = 0xC0;
    
    //initialize Interrupt(state_change)
    IOCAP4 = 1;//rising
    IOCAP5 = 1;
    IOCAF = 0;
    
    IOCCP = 0x00;//falling
    IOCCN = 0x01;
    IOCCF = 0;
    INTCONbits.IOCIE = 1;
    
    //initialize Interrupt(timer1)
    T1CON = 0b01110000; //PRS 1:8 1u sec
    T1CONbits.TMR1ON = 0;
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;
    
    //initialize LCD
    lcd_init();
    lcd_clear();
    lcd_str(StMsg);//line1
    
    SET_CS = 1;
    
    while (1)
    {
        setVol(1,vol);  //SPI LM1972 Ch,Vol
        
        //SSP1BUF = 0x00;
        //SSP1BUF = vol;
        
        lcd_cmd(0xC0);  //set line2
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

//  Interrupt(state_change) 
void __interrupt() isr(void)
{    
    INTCONbits.GIE = 0;
    
    //  Interrupt(state_change En) 
    if(IOCAFbits.IOCAF4 && vol!=127)
    {
        //IOCAFbits.IOCAF4 = 0;
        if(PORTAbits.RA5){ 
            vol++;
            if(vol >= 97) vol_lcd+=10;
            else vol_lcd+=5;
        }
    }
    else if(IOCAFbits.IOCAF5 && vol!=0)
    {
        //IOCAFbits.IOCAF5 = 0;
        if(PORTAbits.RA4){ 
            vol--;
            if(vol >= 96) vol_lcd-=10;
            else vol_lcd-=5;
        }
    }
    IOCAFbits.IOCAF4 = 0;
    IOCAFbits.IOCAF5 = 0;
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
                IOCCP = 0x00;//Falling
                IOCCN = 0x01;
                TMR1 = 0;
                T1CONbits.TMR1ON = 1;
                break;
                
            case 2:
                T1CONbits.TMR1ON = 0;
                if(TMR1>1800 && TMR1<2600)//2200
                {
                    dataReg = preReg1;
                    if(dataReg==0x87 && vol!=127)
                    { 
                        vol++;
                        if(vol >= 97) vol_lcd+=10;
                        else vol_lcd+=5;
                    }
                    else if(dataReg==0xA7 && vol!=0)
                    { 
                        vol--;
                        if(vol >= 96) vol_lcd-=10;
                        else vol_lcd-=5;
                    }
                    state = 0;
                }
                else if(TMR1>4000)
                {
                    IOCCP = 0x01;//Rising
                    IOCCN = 0x00;
                    state = 3;
                }
                break;
                
            case 3:
                if(cnt==31)
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
                cnt = 31;
                IOCCP = 0x00;//Falling
                IOCCN = 0x01;
                dataReg = preReg1;
                if(dataReg==0x87 && vol!=127)
                { 
                    vol++;
                    if(vol >= 97) vol_lcd+=10;
                    else vol_lcd+=5;
                }
                else if(dataReg==0xA7 && vol!=0)
                { 
                    vol--;
                    if(vol >= 96) vol_lcd-=10;
                    else vol_lcd-=5;
                }
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
                    IOCCP = 0x01;//Rising
                    IOCCN = 0x00;
                }
                //INTCONbits.IOCIE = 1;
                break;
                
            case 3:
                if(IRIN){
                    if(cnt>23)
                        preReg4 |= 1 << (cnt-24);
                    else if(cnt<=23 && cnt>15)
                        preReg3 |= 1 << (cnt-16);
                    else if(cnt<=15 && cnt>7)
                        preReg2 |= 1 << (cnt-8);
                    else
                        preReg1 |= 1 << cnt;
                }
                T1CONbits.TMR1ON = 0;
                cnt--;
                //INTCONbits.IOCIE = 1;
                if(cnt==-1){
                    state = 4;
                }
                break;
                
            default:
                break;
        }
    }
    
    INTCONbits.GIE = 1;
}

//  set_volume
void setVol(int ch, int vol_data)
{		
    SET_CS = 0;	
    SDO = 0;	
    SCK = 0;
    
    int i, conv_bit;
    //SPI_Channel
    ch = ((ch-1) & 0x01);   //ch-1 only?
	for (i=7,conv_bit=0x80; i>=0; i--,conv_bit>>=1){
		SDO = (ch & conv_bit) >> i;
		SCK = 1;
		SCK = 0;
	}
    //SPI_Volume
    for(i=7,conv_bit=0x80; i>=0; i--,conv_bit>>=1){ 
        SDO = (vol_data & conv_bit) >> i;
        SCK = 1 ;
        SCK = 0 ;
    }
    SET_CS = 1 ;
}


//  set_LCD
void itostring(int digit, int data, unsigned char *buffer)
{
    int i;
    int x;
    int flag1 = 0;
    int flag2 = 0;
    int flag3 = 0;
    int flag4 = 0;
    buffer += digit;
    
    if(data==5){
        flag2 = 1;
        flag3 = 1;
    }
    if(data==0){
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

void lcd_out(unsigned char code, unsigned char flag){
    LATB = (code & 0xF0);
    if(flag == 0) SET_RS=1;
    else          SET_RS=0;
    SET_E=1;
    SET_E=0;
}

void lcd_data(unsigned char asci){
    lcd_out(asci,0);
    lcd_out(asci<<4,0);
    __delay_us(50);
}

void lcd_cmd(unsigned char cmd){
    lcd_out(cmd,1);
    lcd_out(cmd<<4, 1);
    __delay_ms(2);
}

void lcd_clear(){
    lcd_cmd(0x01);
    __delay_ms(15);
}

void lcd_str(unsigned char *buf){
    while(*buf != 0){
        lcd_data(*buf++);
    }
}

void lcd_init(){
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
