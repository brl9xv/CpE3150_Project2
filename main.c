
#include <asf.h>
int offset=0;
int main (void)
{
	float offset=1;
	DDRA=0x00;  //set PA input
	porta=0xFF;		//pull-up PA resistors
	DDRE=0x10;		//set speaker output
	while(1){
		if(PINA3)
			Middle_C();
		if(PINA4)
			Tenor_C();
		if(PINA5)
			Soprano_C();
		if (PINA6)
			offset= offset-.1;
		if(PINA7)
			offset = offset+.1;
	}
}
void Middle_C(void){  //261.6256 Hz for .1 sec
// delay = (261.6256Hz)^(-1) / 2 *16 M Hz =30578 MC
//cycles for .1 second = .1/(261.6256 Hz)^-1 = 26 cycles
	for(int i = 0; i<26/offset; i++){
		delay(30578*offset);
		PORTE=0x10;
		delay(30578*offset);
		PORTE=0x00;
	}
}

void Tenor_C(void){  //523.2511 Hz for .1sec
// delay = (523.2511Hz)^(-1) / 2 *16 M Hz =15289 MC
//cycles for .1 second = .1/(523.2511 Hz)^-1 = 52 cycles
	for(int i = 0; i<52/offset; i++){
		delay(15289*offset);
		PORTE=0x10;
		delay(15289*offset);
		PORTE=0x00;
	}
}

void Soprano_C(void){  //1046.502 Hz for .1sec
// delay = (1046.502Hz)^(-1) / 2 *16 M Hz =7645 MC
//cycles for .1 second = .1/(1046.502 Hz)^-1 = 105 cycles
	for(int i = 0; i<105/offset; i++){
		delay(7645*offset);
		PORTE=0x10;
		delay(7645*offset);
		PORTE=0x00;
	}
}

void delay(int x){
	for(int i=0; i<x; i++)
}