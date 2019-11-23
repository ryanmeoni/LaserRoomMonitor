/*
 * rmeon001_Lab2_part1.c
 *
 * Created: 10/4/2019 1:39:40 PM
 * Author : Ryan
 */ 
#include <util/delay.h>
#include <avr/io.h>
#include <usart_ATmega1284.h>
#include <timer.h>
#include <bit.h>
unsigned char stageOneInProgress = 0; 
unsigned char stageTwoInProgress = 0; 
unsigned char toggle = 0x00; 
unsigned char L1 = 0; 
unsigned char L2 = 0; 
unsigned char count = 0; 
unsigned char sendCount = 0; 
enum states1 {send} currState1;
void sendData()
{	
	switch (currState1) //Actions
	{
		case send:
		if (USART_IsSendReady(0))
		{
			USART_Send(sendCount, 0); 
		}
			
		//Wait until done sending data
		while (!USART_HasTransmitted(0)) {} 
		
		break; 
	}
}


enum states2 {wait, someoneEntered, increment, hold} currState2;
void checkEnter() 
{
	L1 = (PINB & 0x01); 
	L2 = (PINB & 0x02); 
	
	switch(currState2)
	{
		case wait: 
		//stageOneInProgress = 0; 
		if (L1 && L2) currState2 = wait;
		else if (!L1 && L2) currState2 = someoneEntered; 
		else currState2 = wait; 
		break; 
		
		case someoneEntered: 
		//stageOneInProgress = 1; 
		if (!L1 && L2) currState2 = someoneEntered; 
		else if (!L1 && !L2) currState2 = increment;
		else currState2 = wait;  
		break; 
		
		case increment: 
		//stageOneInProgress = 1; 
		sendCount++; 
		currState2 = hold; 
		break; 
		
		case hold: 
		//stageOneInProgress = 1; 
		if (!L1 && !L2) currState2 = hold; 
		else if (L1 && L2) currState2 = wait;
		else currState2 = wait;  
		break; 
		
		default: 
		currState2 = wait; 
		
	}
}

enum states3 {waitLeave, someoneLeft, incrementLeave, holdLeave} currState3;
void checkLeave()
{
	switch(currState3)
	{
		case waitLeave:
		//stageTwoInProgress = 0; 
		if (L1 && L2) currState3 = waitLeave;
		else if (L1 && !L2) currState3 = someoneLeft;
		else currState3 = waitLeave; 
		break;
		
		case someoneLeft:
		//stageTwoInProgress = 1; 
		if (L1 && !L2) currState3 = someoneLeft;
		else if (!L1 && !L2) currState3 = incrementLeave;
		else currState3 = waitLeave;
		break;
		
		case incrementLeave:
		//stageTwoInProgress = 1; 
		if (sendCount > 0) sendCount--;
		currState3 = holdLeave;
		break;
		
		case holdLeave:
		//stageTwoInProgress = 1; 
		if (!L1 && !L2) currState3 = holdLeave;
		else if (L1 && L2) currState3 = waitLeave;
		else currState3 = waitLeave;
		break;
		
		default:
		currState3 = waitLeave;
	}
}

unsigned char portCval = 0x00; 
void checkLasersSetOutputs()
{
	unsigned char LaserDetectorOne = (PINB & 0x01);
	unsigned char LaserDetectorTwo = (PINB & 0x02);
	
	if (LaserDetectorOne) portCval = SetBit(portCval, 1, 1);
	else if (!LaserDetectorOne) portCval = SetBit(portCval, 1, 0);
	
	if (LaserDetectorTwo) portCval = SetBit(portCval, 0, 1);
	else if (!LaserDetectorTwo) portCval = SetBit(portCval, 0, 0);
	
	PORTC = portCval;
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
	
	currState1 = send; 
	currState2 = wait; 
	currState3 = waitLeave; 
	
    /* Replace with your application code */
    while (1) 
    { 
		checkEnter(); 
		checkLeave(); 
		checkLasersSetOutputs();
		sendData(); 
		_delay_ms(50);
    }
}
