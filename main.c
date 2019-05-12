/*
 * GccApplication4.c
 *
 * Created: 5/12/2019 5:48:42 PM
 * Author : Brendan Liebhart
 */ 

#include <avr/io.h>


#include <avr/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//	Predeclaration		//

//void Middle_C();
//void Tenor_C();
//void Soprano_C();
void playNote(int frequency, int length);
void playSong(int * noteArray, int length);
void SetDelay(long x);
void Tune1();
void Tune2();

//	Frequency Constants		//

const int C3 = 131;
const int D3 = 147;
const int E3 = 165;
const int F3 = 175;
const int G3 = 196;
const int A3 = 220;
const int B3 = 247;
const int C4 = 262;
const int D4 = 294;
const int E4 = 330;
const int F4 = 349;
const int G4 = 392;
const int A4 = 440;
const int B4 = 494;
const int C5 = 523;
const int D5 = 587;
const int E5 = 659;
const int F5 = 698;
const int G5 = 784;
const int A5 = 880;
const int B5 = 988;
const int C6 = 1047;
const int D6 = 1175;
const int D4_E4=311;
const int A4_B4=466;
const int pause = 0;

//	Song Constants	//
// int arrays with frequency, note length

//	Mary had a little lamb
int song1[52] = {
	988, 4,
	880, 4,
	784, 4,
	880, 4,
	988, 4,
	988, 4,
	988, 2,
	880, 4,
	880, 4,
	880, 2,
	988, 4,
	1175, 4,
	1175, 2,
	988, 4,
	880, 4,
	784, 4,
	880, 4,
	988, 4,
	988, 4,
	988, 4,
	988, 4,
	880, 4,
	880, 4,
	988, 4,
	880, 4,
	784, 2,
};

int song2[124] = {
	659, 4,//e
	659, 4,
	698, 4,//f
	784, 4,//g
	784, 4,
	698, 4,
	659, 4,
	587, 4,//d
	523, 4,//c
	523, 4,
	587, 4,
	659, 4,
	659, 4,
	587, 4,
	587, 2,//d,half
	659, 4,
	659, 4,
	698, 4,
	784, 4,
	784, 4,
	698, 4,
	659, 4,
	587, 4,
	523, 4,
	523, 4,
	587, 4,
	659, 4,
	587, 4,
	523, 4,
	523, 2,//c,half
	587, 4,
	587, 4,
	659, 4,
	523, 4,
	587, 4,
	659, 8,
	698, 8,
	659, 4,
	523, 4,
	587, 4,
	659, 8,
	698, 8,
	659, 4,
	587, 4,
	523, 4,
	587, 4,
	0, 2,
	659, 4,
	659, 4,
	698, 4,
	784, 4,
	784, 4,
	698, 4,
	659, 4,
	587, 4,
	523, 4,
	523, 4,
	587, 4,
	659, 4,
	587, 4,
	523, 4,
	523, 4
};

//	Global Variables	//

int offset=100;
int MusicCycles=0;
int DelayTime;
int nextNote;

//	Main	//

int main (void)
{
	DDRA=0x00;		// Set PA input
	PORTA=0xFF;		// Set PA pull-up resistors
	DDRD=0xFF;		// Set PD output
	DDRE=0x10;		// Set speaker output
	sei();			// Set global interrupts
	while(1){
		unsigned char temp = ~PINA;
		if(temp & 0b00000001)
			playNote(C4,4);
		if(temp & 0b00000010)
			playSong(song1,52);
		if(temp & 0b00000100)
			playSong(song2,124);
		if (temp & 0b00010000)
			offset= offset-5;
		if(offset==0)
			offset=5;
		if(temp & 0b0010000)
			offset = offset+5;
		if(temp & 0b1000000)
			playSong(song1,52);
	}
}

// plays passed note for length
// use note constants for frequency
// length gives division factor (e.g. 1 is whole note, 2 is half note, etc.)
// e.g. playNote(c4, 1)
void playNote(int frequency, int length)
{
	if(!frequency)
	{
		for(int i=0; i<100/length; i+=1)
			_delay_ms(100);
	}

	SetDelay((16000000/frequency)*2);
	MusicCycles = frequency/length;//(offset/100);
	TIMSK0 = 0x01;
	nextNote = 0;

	return;
}

void playSong(int * noteArray, int length)
{
	for(int i=0; i<length; i+=2)
	{
		playNote(noteArray[i],noteArray[i+1]);
		while(!nextNote)
			_delay_ms(1);
	}
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
	else
		nextNote=1;
}

