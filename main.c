
void Middle_C();
void Tenor_C();
void Soprano_C();
void SetDelay(long x);
void Tune1();

#include <asf.h>
int offset=100;
int MusicCycles=0;
int DelayTime;
int main (void)
{
	DDRA=0x00;  //set PA input
	PORTA=0xFF;		//pull-up PA resistors
	DDRD=0xFF;
	DDRE=0x10;		//set speaker output
	while(1){
		unsigned char temp = ~PINA;
		if(temp & 0b00000001)
			Middle_C();
		if(temp & 0b00000010)
			Tenor_C();
		if(temp & 0b00001000)
			Soprano_C();
		if (temp & 0b00010000)
			offset= offset-5;
		if(offset==0)
			offset=5;
		if(temp & 0b0010000)
			offset = offset+5;
		if(temp & 0b0100000)
			Tune1();
	}
}
void Middle_C(){  //261.6256 Hz for .1 sec
	// delay = (261.6256Hz)^(-1) / 2 *16 M Hz =30578 MC
	//cycles for .1 second = .1/(261.6256 Hz)^-1 = 26 cycles
	SetDelay(30578*(offset/100));
	MusicCycles=(26/(offset/100));
	TIMSK0=0x01;
	sei();
}

void Tenor_C(){  //523.2511 Hz for .1sec
	// delay = (523.2511Hz)^(-1) / 2 *16 M Hz =15289 MC
	//cycles for .1 second = .1/(523.2511 Hz)^-1 = 52 cycles
	SetDelay(15289*(offset/100));
	MusicCycles=(52/(offset/100));
	TIMSK0=0x01;
	sei();
}

void Soprano_C(){  //1046.502 Hz for .1sec
	// delay = (1046.502Hz)^(-1) / 2 *16 M Hz =7645 MC
	//cycles for .1 second = .1/(1046.502 Hz)^-1 = 105 cycles
	SetDelay(7645*(offset/100));
	MusicCycles=(105/(offset/100));
	TIMSK0=0x01;
	sei();

}

void Tune1(){
	G4(2);
	pause(1);
	G4(2);
	pause(1);
	G4(2);
	pause(1);
	
	D4_E4(1);
	A4-B4(1);
	G4(2);
	pause(1);
	D4-E4(1);
	A4-B4(1);
	
	D5(2);
	pause(1);
	D5(2);
	pause(1);
	D5(2);
	pause(1);
	
	D4_E4(1);
	A4-B4(1);
	G4(2);
	pause(1);
	D4-E4(1);
	A4-B4(1);
	
}

void SetDelay(long x){
	if(x>261120)
		return;//too big for the timer
	else if(x>65280){
		DelayTime=-(x/1024);
		TCNT0=-(x/1024);
		TCCR0B=0x05;
	}
	else if(x>16320){
		DelayTime=-(x/256);
		TCNT0=-(x/256);
		TCCR0B=0x04;
	}
	else if(x>2040){
		DelayTime=-(x/64);
		TCNT0=-(x/64);
		TCCR0B=0x03;
	}
	else if(x>255){
		DelayTime=-(x/8);
		TCNT0=-(x/8);
		TCCR0B=0x02;
	}
	else if(x>0){
		DelayTime=-x;
		TCNT0=-(x);
		TCCR0B=0x01;
	}
	PORTD=0xF0;
	return;
}

ISR(TIMER0_OVF_vect){
	PORTD=0x0F;
	TCNT0=DelayTime;
	if(MusicCycles>0){
		MusicCycles-=1;
		PORTE^=0x10;
	}
}
