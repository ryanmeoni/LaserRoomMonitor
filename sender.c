/*
 * rmeon001_Lab2_part1.c
 *
 * Created: 10/4/2019 1:39:40 PM
 * Author : Ryan
 */ 

#include <avr/io.h>
#include <usart_ATmega1284.h>
#include <timer.h>
#include <bit.h>
unsigned char stageOneInProgress = 0; 
unsigned char stageTwoInProgress = 0; 
unsigned char toggle = 0x00; 
unsigned char count = 0; 
unsigned char sendCount = 0; 
enum states1 {toggleLED} currState1;
void sendData()
{	
	switch (currState1) //Actions
	{
		case toggleLED: 
		
		if (count == 10)
		{
			if (toggle == 0x01) toggle = 0x00;
			else if (toggle == 0x00) toggle = 0x01;
			
			if (USART_IsSendReady(0))
			{
				USART_Send(sendCount, 0); 
				sendCount++; 
				PORTB = toggle;
			}
			
			//Wait until done sending data
			while (!USART_HasTransmitted(0)) {}
			count = 0; 
		}
		
		else {
			count++; 
		}
		
		break; 
	}
}

int main(void)
{
	DDRB = 0x00; 
	PORTB = 0xFF; 
	DDRC = 0xFF; 
	PORTC = 0x00; 
	
	initUSART(0); 
	TimerSet(100); 
	TimerOn(); 
	
	currState1 = toggleLED; 
	
    /* Replace with your application code */
    while (1) 
    {
		sendData(); 
		while(!TimerFlag) {}
		TimerFlag = 0; 
    }
}
