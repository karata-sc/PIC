#include "mcc_generated_files/mcc.h"
#define LED RC2
// FOSC = INTOSC (8MHz)

int cnt = 0;
int flag=0;

void main(void)
{
	SYSTEM_Initialize();

	OPTION_REG = 0b10000111;
	TMR0 = 248;
	INTCON = 0b11100100;　//TMR0IF = 0で0b11100000では？
	while(1)　LED = flag;
}

// Interrupt(timer0)
void __interrupt() isr(void)
{
	//GIE = 0; 割り込み中に割り込まれないようにクリア？
	if(TMR0IF){
		TMR0 = 248;
		cnt++;
		TMR0IF = 0;
		if(cnt>=100)
		{
			cnt=0;
			flag = ~flag;
		}
	}
	//GIE = 1;
}

/**
End of File
*/
