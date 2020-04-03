#include <avr/io.h>
#include "lcd.h"
#include "keypad.h"
#include "timer.h"
#include "usart_ATmega1284.h"
#define usart0 0

enum States {Start, Idle, keyOne, keyTwo, keyThree, keyFour, keyFive, keySix, keySeven, keyEight, keyNine, keyZero, keyAsterisk, keyHashtag, Transmit, keyB, keyC, keyD, Receive, Wait} State;

unsigned char pressedKey, key, cursorPosition, i, firstFlag, secondFlag, thirdFlag, character, _index, received;
const unsigned char BUFFER_SIZE = 32; //16x2 LCD
char buffer[33] = {0,0,0,0,0,0,0,0,0,0,0,
		   0,0,0,0,0,0,0,0,0,0,0,
		   0,0,0,0,0,0,0,0,0,0,0};

void readInput()
{
	key = GetKeypadKey();

	if (!key)
		pressedKey = 0;
	else
		pressedKey = 1;
}

void LCD_shiftCursorLeft() {LCD_WriteCommand(0x10);}

void deleteCharacter()
{
	if (cursorPosition > 1)
	{
		LCD_shiftCursorLeft();
		LCD_WriteData(' ');
		LCD_shiftCursorLeft();
		LCD_Cursor(--cursorPosition);
	}
}

void writeCharacter(unsigned char character, unsigned char deleteChar)
{
	if (deleteChar)
		deleteCharacter();

	LCD_Cursor(cursorPosition++);
	LCD_WriteData(character);
}

void zeroFlags() {i = 0; firstFlag = 0; secondFlag = 0; thirdFlag = 0;}

void idleState_zeroFlags_appendChar()
{
	State = Idle;

	zeroFlags();

	if (_index < BUFFER_SIZE)
		buffer[_index++] = character;
}

void init()
{
	_index = 0; pressedKey = 0; character = 0; key = 0; received = 0;
	zeroFlags();
	cursorPosition = 1;
}

void clearBuffer()
{
	unsigned char position;
	_index = 0;
	for (position = 0; position < BUFFER_SIZE; position++)
		buffer[position] = 0;
}

void clearScreen()
{
	cursorPosition = 1;
	LCD_ClearScreen();
}

void ifReceived()
{
	if (received)
	{
		received = 0;
		clearScreen();
	}
}

void TickFunction()
{
	switch (State) //state transitions
	{
		case Idle:
		{
			if (USART_HasReceived(usart0))
			{
				clearBuffer();
				clearScreen();
				received = 1;
				State = Receive;
			}
			else
			{
				if (pressedKey)
				{
					switch (key)
					{
						case '0': ifReceived(); State = keyZero; break;
						case '1': ifReceived(); State = keyOne; break;
						case '2': ifReceived(); State = keyTwo; break;
						case '3': ifReceived(); State = keyThree; break;
						case '4': ifReceived(); State = keyFour; break;
						case '5': ifReceived(); State = keyFive; break;
						case '6': ifReceived(); State = keySix; break;
						case '7': ifReceived(); State = keySeven; break;
						case '8': ifReceived(); State = keyEight; break;
						case '9': ifReceived(); State = keyNine; break;
						case '*': ifReceived(); State = keyAsterisk; break;
						case '#': ifReceived(); State = keyHashtag; break;
						case 'B': ifReceived(); State = keyB; break;
						case 'C': ifReceived(); State = keyC; break;
						case 'D': ifReceived(); State = keyD; break;

						case 'A':
						{
							ifReceived();

							if (USART_IsTransmitReady(usart0))
							{
								_index = 0;
								State = Transmit;
							}
							
							break;
						}

						default: ifReceived(); State = Idle; break;
					}
				}
				else
					State = Idle;
			}

			break;
		}

		case keyZero:
		{
			if (pressedKey && key == '0')
				State = keyZero;
			else
				idleState_zeroFlags_appendChar();
			
			break;
		}

		case keyOne:
		{
			if (pressedKey && key == '1')
				State = keyOne;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyTwo:
		{
			if (pressedKey && key == '2')
				State = keyTwo;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyThree:
		{
			if (pressedKey && key == '3')
				State = keyThree;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyFour:
		{
			if (pressedKey && key == '4')
				State = keyFour;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyFive:
		{
			if (pressedKey && key == '5')
				State = keyFive;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keySix:
		{
			if (pressedKey && key == '6')
				State = keySix;
			else
				idleState_zeroFlags_appendChar();

			break;
		}
		case keySeven:
		{
			if (pressedKey && key == '7')
				State = keySeven;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyEight:
		{
			if (pressedKey && key == '8')
				State = keyEight;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyNine:
		{
			if (pressedKey && key == '9')
				State = keyNine;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyAsterisk:
		{
			if (pressedKey && key == '*')
				State = keyAsterisk;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case keyHashtag:
		{
			if (pressedKey && key == '#')
				State = keyHashtag;
			else
				idleState_zeroFlags_appendChar();

			break;
		}

		case Transmit:
		{
			if (USART_HasTransmitted(usart0))
			{
				if (!buffer[_index])
				{
					USART_Transmit(0, usart0);
					clearScreen();
					clearBuffer();
					State = Idle;
				}
				else
				State = Transmit;
			}
			else
			{
				_index--;
				State = Transmit;
			}

			break;
		}

		case Receive:
		{
			if (USART_HasReceived(usart0))
				State = Receive;
			else
			{
				i = 0;
				State = Wait;
			}

			break;
		}

		case Wait:
		{
			if (USART_HasReceived(usart0))
			{
				received = 1;
				State = Receive;
			}
			else
			{
				if (i == 5) //1 sec
				{
					if (_index < BUFFER_SIZE)
						buffer[_index] = 0;

					i = 0;

					LCD_DisplayString(1, buffer);
					clearBuffer();

					State = Idle;
				}
				else
					State = Wait;
			}

			break;
		}

		case keyB: State = Idle; break;
		case keyC: State = Idle; break;
		case keyD: State = Idle; break;
		case Start: init(); State = Idle; break;
		default: State = Start; break;
	}





	switch (State) //state actions
	{
		case keyZero:
		{
			if (!i)
			{
				LCD_Cursor(cursorPosition++);
				LCD_WriteData(key);
				character = key;
			}

			i++;

			readInput();

			break;
		}

		case keyOne:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'A';
					writeCharacter('A', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'B';
					writeCharacter('B', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'C';
					writeCharacter('C', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '1';
					writeCharacter('1', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyTwo:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'D';
					writeCharacter('D', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'E';
					writeCharacter('E', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'F';
					writeCharacter('F', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '2';
					writeCharacter('2', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyThree:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'G';
					writeCharacter('G', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'H';
					writeCharacter('H', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'I';
					writeCharacter('I', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '3';
					writeCharacter('3', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyFour:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'J';
					writeCharacter('J', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'K';
					writeCharacter('K', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'L';
					writeCharacter('L', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '4';
					writeCharacter('4', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyFive:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'M';
					writeCharacter('M', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'N';
					writeCharacter('N', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 0xEE;
					writeCharacter(0xEE, 1); //~n
					thirdFlag = 0;
				}
				else
				{
					character = '5';
					writeCharacter('5', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keySix:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'O';
					writeCharacter('O', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'P';
					writeCharacter('P', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'Q';
					writeCharacter('Q', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '6';
					writeCharacter('6', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keySeven:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'R';
					writeCharacter('R', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'S';
					writeCharacter('S', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'T';
					writeCharacter('T', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '7';
					writeCharacter('7', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyEight:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'U';
					writeCharacter('U', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'V';
					writeCharacter('V', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'W';
					writeCharacter('W', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '8';
					writeCharacter('8', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyNine:
		{
			if (!i)
			{
				character = key;
				writeCharacter(key, 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = 'X';
					writeCharacter('X', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = 'Y';
					writeCharacter('Y', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = 'Z';
					writeCharacter('Z', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '9';
					writeCharacter('9', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyAsterisk:
		{
			if (!i)
			{
				character = '.';
				writeCharacter('.', 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = '!';
					writeCharacter('!', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = '?';
					writeCharacter('?', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = ',';
					writeCharacter(',', 1);
					thirdFlag = 0;
				}
				else
				{
					character = '.';
					writeCharacter('.', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case keyHashtag:
		{
			if (!i)
			{
				character = ' ';
				writeCharacter(' ', 0);
				firstFlag = 1;
			}

			if (!(i % 5) && i > 0)
			{
				if (firstFlag)
				{
					character = '\'';
					writeCharacter('\'', 1);
					firstFlag = 0; secondFlag = 1;
				}
				else if (secondFlag)
				{
					character = '&';
					writeCharacter('&', 1);
					secondFlag = 0; thirdFlag = 1;
				}
				else if (thirdFlag)
				{
					character = '$';
					writeCharacter('$', 1);
					thirdFlag = 0;
				}
				else
				{
					character = ' ';
					writeCharacter(' ', 1);
					firstFlag = 1;
				}
			}

			i++;

			readInput();

			break;
		}

		case Transmit:
		{
			if (_index < BUFFER_SIZE)
				USART_Transmit(buffer[_index++], usart0);
			
			break;
		}

		case Receive:
		{
			if (_index < BUFFER_SIZE)
			{
				buffer[_index++] = USART_Receive(usart0);
				USART_Flush(usart0);
			}

			break;
		}

		case keyB: //newline character
		{
			if (cursorPosition > 1 && cursorPosition < 17)
			{
				cursorPosition = 17;
				LCD_Cursor(cursorPosition);

				_index = 16;
				for (unsigned char posix = _index; posix < 16; posix++)
					buffer[posix] = ' ';
			}

			break;
		}

		case keyC: //clear
		{
			if (*buffer != 0)
			{
				clearScreen();
				clearBuffer();
			}
			
			break;
		}

		case keyD: //delete a character
		{
			deleteCharacter();

			if (_index > 0)
				buffer[--_index] = 0;

			break;
		}

		case Wait: i++; break;
		case Idle: readInput(); break;
		default: State = Start; break;
	}
}

int main()
{
	DDRA = 0xF0;
	PORTA = 0x0F; //for keypad

	//PORTB is free

	DDRC = 0xFF;
	PORTC = 0x00; // LCD data lines

	DDRD = 0xFA; //1111 1010
	PORTD = 0x05; // LCD control lines and USART 0000 0101

	State = Start;

	initUSART(usart0);
	LCD_init();

	TimerSet(200);
	TimerOn();

	while (1)
	{
		TickFunction();
		while (!TimerFlag);
		TimerFlag = 0;
	}

	return 0;
}