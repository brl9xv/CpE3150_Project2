#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

//	Predeclaration		//
void USART_Init(unsigned long BAUDRATE);                // initialize USART function
void playNote(int frequency, int length);
void playSong(int * noteArray, int length);
void SetDelay(long x);
void Tune1();
void Tune2();
void USART_TxChar (unsigned char data);
void USART_SendString(unsigned char *str);
ISR(USART1_RX_vect);
void playInsertSong(unsigned char *song);
unsigned char value;
int tempo = 0;
const int tempoList[4] = {1,2,3,4};
unsigned char inputstring[24];

#define F_CPU 16000000UL

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
int operation =1;
int offset=100;
int MusicCycles=0;
int DelayTime;
int nextNote;

//	Main	//

int main (void)
{
	DDRA=0x00;		// Set PA input
	PORTA=0xFF;		// Set PA pull-up resistors
	DDRD=0xFB;		// Set PD output	PORTD |= 0xFF;
	DDRE=0x10;		// Set speaker output
	
	USART_Init(9600);
	UCSR1B |= (1 << RXCIE);    // receive interrupt enabled
	sei();			// Set global interrupts
	
	USART_SendString("\nSong Mode:\nButton 1 = Mary had a little lamb\nButton 2 = Ode to Joy\nButton 3 = Stop playback\nEnter 1-4 to change tempo multiplier\nEnter n for single note mode\nEnter 'I' to change to input song mode\n");
	
	while(1)
	{
		unsigned char temp = ~PINA;
		
		if(temp & 0b00000001 && operation==1)
		{
			USART_SendString("Now Playing: Mary had a Little Lamb\n");
			playSong(song1,52);
		}
		if(temp & 0b00000010 && operation==1)
		{
			USART_SendString("Ode to Joy\n");
			playSong(song2,124);
		}
		if (temp & 0b00010000)
			offset= offset-5;
		if(offset==0)
			offset=5;
		if(temp & 0b0010000)
			offset = offset+5;
		if(temp & 0b00000001 && operation==3)
		{
			strcat(inputstring, '\0');
			USART_SendString("Playing Notes:\t");
			USART_SendString(inputstring);
			USART_SendString("\n");
			playInsertSong(inputstring);
		}
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
		for(int i=0; i<(100/length)/tempoList[tempo]; i+=1)
		_delay_ms(100);
	}

	SetDelay((16000000/frequency*(offset/100))*2);
	MusicCycles = (frequency/(offset/100)/length)/tempoList[tempo];
	TIMSK0 = 0x01;
	nextNote = 0;

	return;
}

void playSong(int * noteArray, int length)
{
	for(int i=0; i<length; i+=2)
	{
		if((operation==1 || operation==3) && !(~PINA & 0b00000100))
		{
			
			playNote(noteArray[i],noteArray[i+1]);
			while(!nextNote)
			_delay_ms(1);
		}
		else
		{
			i =length;
		}
	}
}

void SetDelay(long x)
{
	if(x>261120)
	return;//too big for the timer
	else if(x>65280)
	{
		DelayTime=-(x/1024);
		TCNT0=-(x/1024);
		TCCR0B=0x05;
	}
	else if(x>16320)
	{
		DelayTime=-(x/256);
		TCNT0=-(x/256);
		TCCR0B=0x04;
	}
	else if(x>2040)
	{
		DelayTime=-(x/64);
		TCNT0=-(x/64);
		TCCR0B=0x03;
	}
	else if(x>255)
	{
		DelayTime=-(x/8);
		TCNT0=-(x/8);
		TCCR0B=0x02;
	}
	else if(x>0)
	{
		DelayTime=-x;
		TCNT0=-(x);
		TCCR0B=0x01;
	}
	return;
}

ISR(TIMER0_OVF_vect)
{
	TCNT0=DelayTime;
	if(MusicCycles>0){
		MusicCycles-=1;
		PORTE^=0x10;
	}
	else
	nextNote=1;
}

void USART_Init(unsigned long BAUDRATE)                // USART initialize function */
{
	int BAUD_PRESCALE = (((F_CPU / (BAUDRATE * 16UL))) - 1);      // Define prescale

	UCSR1B |= (1 << RXEN) | (1 << TXEN) ;            // Enable USART transmitter and receiver
	UCSR1C |= (1 << UCSZ0) | (1 << UCSZ1);            // Write USCRC for 8 bit data and 1 stop bit, asynchronous

	UBRR1L = BAUD_PRESCALE;                            // Load UBRRL with lower 8 bit of prescale
	UBRR1H = (BAUD_PRESCALE >> 8);                    // Load UBRRH with upper 8 bit of prescale
}

// Data transmitting function
void USART_TxChar(unsigned char data)
{
	UDR1 = data;                                        /* Write data to be transmitting in UDR */
	while (!(UCSR1A & (1<<UDRE)));                    /* Wait until data transmit and buffer get empty */
}


// used to send a string of characters to the USART (instead of one character at a time).
void USART_SendString(unsigned char *str)
{
	int i=0;
	while (str[i]!='\0')
	{
		USART_TxChar(str[i]);                        /* Send each char of string till the NULL */
		i++;
	}
}

ISR(USART1_RX_vect)
{
	value = UDR1; // UDR1 is register with received byte
	if (operation == 3)
	{
		char str[2];
		str[0] = value;
		str[1] = '\0';
		strcat(inputstring, str);
	}

	switch(value)
	{
		case 'S':
			if(operation!=1)
				USART_SendString("\nSong Mode:\nButton 1 = Mary had a little lamb\nButton 2 = Ode to Joy\nButton 3 = Stop playback\nEnter 1-4 to change tempo multiplier\nEnter n for single note mode\nEnter 'I' to change to input song mode\n");
			operation = 1;
			break;
		case 'N':
			if(operation!=2)
				USART_SendString("\nSingle Note Mode:\n");
			tempo = 0;
			operation = 2;
			break;
		case '1':
			if(operation==1)
			{
				tempo = 0;
				USART_SendString("1x Tempo\n");
			}
			break;
		case '2':
			if(operation==1)
			{
				tempo = 1;
				USART_SendString("2x Tempo\n");
			}
			break;
		case '3':
			if(operation==1)
			{
				tempo = 2;
				USART_SendString("3x Tempo\n");
			}
			break;
		case '4':
			if(operation==1)
			{
				tempo = 3;
				USART_SendString("4x Tempo\n");
			}
			break;
		case 'I':
			memset(inputstring, 0, sizeof(inputstring));
			if(operation!=3)
				USART_SendString("Note Input Mode\nType the note letters that you want played then note by note then press button 1 to play song\n");
			operation=3;
			break;
	}
}

void playInsertSong(unsigned char *song)
{
	int songplay[50];
	int i = 0;
	int temp = 0;
	while (song[i]!='\0')
	{
		switch(song[i])
		{
			case 'A':
			{
				songplay[(i*2)] = 880;
				songplay[(i*2)+1] = 4;
				break;
			}
			case 'B':
			{
				songplay[(i*2)] = 988;
				songplay[(i*2)+1] = 4;
				break;
			}
			case 'C':
			{
				songplay[(i*2)] = 523;
				songplay[(i*2)+1] = 4;
				break;
			}
			case 'D':
			{
				songplay[(i*2)] = 587;
				songplay[(i*2)+1] = 4;
				break;
			}
			case 'E':
			{
				songplay[(i*2)] = 659;
				songplay[(i*2)+1] = 4;
				break;
			}
			case 'F':
			{
				songplay[(i*2)] = 698;
				songplay[(i*2)+1] = 4;
				break;
			}
			case 'G':
			{
				songplay[(i*2)] = 784;
				songplay[(i*2)+1] = 4;
				break;
			}
		}
		i++;
	}
	temp = (i) * 2;
	playSong(songplay, temp);
}
