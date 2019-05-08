
void Middle_C();
void Tenor_C();
void Soprano_C();
void SetDelay(long x);

#include <asf.h>
int offset=1;
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
		{
		PORTD=0xFF;
		Middle_C();
		}
		if(temp & 0b00000010)
		Tenor_C();
		if(PINA5==0)
		Soprano_C();
		if (temp & 0b00000100)
		offset= offset;
		if(temp & 0b00000100)
		offset = offset;
	}
}
void Middle_C(){  //261.6256 Hz for .1 sec
	// delay = (261.6256Hz)^(-1) / 2 *16 M Hz =30578 MC
	//cycles for .1 second = .1/(261.6256 Hz)^-1 = 26 cycles
	SetDelay(30578*offset);
	MusicCycles=(26/offset);
	TIMSK0=0x01;
	sei();
}

void Tenor_C(){  //523.2511 Hz for .1sec
	// delay = (523.2511Hz)^(-1) / 2 *16 M Hz =15289 MC
	//cycles for .1 second = .1/(523.2511 Hz)^-1 = 52 cycles
	SetDelay(15289*offset);
	MusicCycles=(52/offset);
	TIMSK0=0x01;
	sei();
}

void Soprano_C(){  //1046.502 Hz for .1sec
	// delay = (1046.502Hz)^(-1) / 2 *16 M Hz =7645 MC
	//cycles for .1 second = .1/(1046.502 Hz)^-1 = 105 cycles
	SetDelay(7645*offset);
	MusicCycles=(105/offset);
	TIMSK0=0x01;
	sei();

}

void SetDelay(long x){
	if(x>65280){
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
