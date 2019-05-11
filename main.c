#include <asf.h>

//	Predeclaration		//

//void Middle_C();
//void Tenor_C();
//void Soprano_C();
void playNote(int frequency, int length);
void SetDelay(long x);
void Tune1();
void Tune2();

//	Frequency Constants		//

int C3 = 131;
int D3 = 147;
int E3 = 165;
int F3 = 175;
int G3 = 196;
int A3 = 220;
int B3 = 247;
int C4 = 262;
int D4 = 294;
int E4 = 230;
int F4 = 349;
int G4 = 392;
int A4 = 440;
int B4 = 494;
int C5 = 523;
int D5 = 587;
int E5 = 659;
int F5 = 698;
int G5 = 784;
int A5 = 880;
int B5 = 988;
int C6 = 1047;
int D4_E4=311;
int A4_B4=466;
int pause=1;

//	Global Variables	//

int offset=100;
int MusicCycles=0;
int DelayTime;

//	Main	//

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
		if(temp & 0b1000000)
			Tune2();
	}
}

/*
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
*/

// plays passed note for length
// use note constants for frequency
// length gives division factor (e.g. 1 is whole note, 2 is half note, etc.)
// e.g. playNote(c4, 1)
void playNote(int frequency, int length)
{
	int delay = 16000000*(2/frequency)*(offset/100);
	SetDelay(delay);
	MusicCycles = frequency/length/10/(offset/100);
	TIMSK0 = 0x01;
	sei();
	
	while(MusicCycles!=0){
		unsigned char temp = ~PINA;
		if(temp & 0b00000001)
			return;				//stop the sound
		if(temp & 0b00000010)
			return;				//stop the sound,can be use for personal feature
		if(temp & 0b00001000)
			return;				//stop the sound,can be use for personal feature
		if (temp & 0b00010000)
			offset= offset-5;
		if(offset==0)
			offset=5;
		if(temp & 0b0010000)
			offset = offset+5;
		if(temp & 0b0100000)
			return;				//stop the sound,can be use for personal feature
	}
	return;
}

void Tune1(){
	playNote(G4,2);
	playNote(pause,1);
	playNote(G4,2);
	playNote(pause,1);
	playNote(G4,2);
	playNote(pause,1);
	
	playNote(D4_E4,1);
	playNote(A4_B4,1);
	playNote(G4,2);
	playNote(pause,1);
	playNote(D4_E4,1);
	playNote(A4_B4,1);
	
	playNote(D5,2);
	playNote(pause,1);
	playNote(D5,2);
	playNote(pause,1);
	playNote(D5,2);
	playNote(pause,1);
	
	playNote(D4_E4,1);
	playNote(A4_B4,1);
	playNote(G4,2);
	playNote(pause,1);
	playNote(D4_E4,1);
	playNote(A4_B4,1);
	
}

void Tune2(){

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
