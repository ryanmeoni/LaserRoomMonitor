#ifndef F_CPU
#define F_CPU 8000000UL //define microcontroller clock speed
#endif

#include <util/delay.h>
#include "nokia5110.c" //library for my nokia5110 display 
#include "DHT.c" //library that works with DHT sensor
#include <string.h>
#include <stdlib.h>
#include <usart_ATmega1284.h>

uint16_t temperature_int = 0;
uint16_t humidity_int = 0;

//function to convert number values into characters

int main(void) {
	DDRB = 0b11111110;
	PORTB = 0b00000001; 
	
	//Initializing LCD Display to output data from DHT sensor
	nokia_lcd_init();
	initUSART(0);
	USART_Flush(0); 
	
	//Setup fields for temp/humidity 
	char humidityBuffer[100] = {0}; 
	char temperatureBuffer[100] = {0};
	char countBuffer[100] = {0}; 
	char* humidityPrint = "Humidity: ";
	char* temperaturePrint = "Temp: ";
	char* percentString = "%"; 
	char* celciusString = "C"; 
	
	unsigned char portBval = 0x00; 
	unsigned char countRecieve = 0; 

    while (1) {
		
		//DO TEMPERATURE SENSING STUFF
		
		//Allocate space for our 2 strings to Nokia LCD
		char *humidityString = malloc(strlen(humidityPrint) + strlen(humidityBuffer) + strlen(percentString) + 1); // +1 for the null-terminator
		char *temperatureString = malloc(strlen(temperaturePrint) + strlen(temperatureBuffer) + strlen(celciusString) + 1); // +1 for the null-terminator
		
		//Update our temp and humidity readings 
		if (dht_GetTempUtil(&temperature_int, &humidity_int) != -1) 
		{
			temperature_int = temperature_int / 10; 
			humidity_int = humidity_int / 10; 
			
			sprintf(humidityBuffer, "%d", humidity_int);
			sprintf(temperatureBuffer, "%d", temperature_int); 
			
			//Copy data over to humidity string
			strcpy(humidityString, humidityPrint);
			strcat(humidityString, humidityBuffer);
			strcat(humidityString, percentString); 
			
			//Copy data over to temperature string 
			strcpy(temperatureString, temperaturePrint); 
			strcat(temperatureString, temperatureBuffer); 
			strcat(temperatureString, celciusString); 
			
			//Display to Nokia LCD
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string(humidityString, 1);
			nokia_lcd_set_cursor(0, 20);
			nokia_lcd_write_string(temperatureString, 1);
			sprintf(countBuffer, "%d", countRecieve);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string(countBuffer, 1);
			nokia_lcd_render();
			
			//Turn on fan if it's too hot (using humidity because varying temperature is too hard)
			if (humidity_int > 65)
			{
				portBval = 0x02; 
				PORTB = portBval; 
			}
			else 
			{
				portBval = 0x00; 
				PORTB = portBval; 
			}
		}
		
		else 
		{
			//Print error message, if temp/humidity sensor isn't working right 
			/*nokia_lcd_clear();
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Error", 1);
			nokia_lcd_render();*/
		}
		
		//Free our dynamically allocated memory
		free(humidityString); 
		free(temperatureString); 
		
		//DO OUR USART RECIEVE STUFF
		
		if (USART_HasReceived(0))
		{
			countRecieve = USART_Receive(0); 
			USART_Flush(0);
			sprintf(countBuffer, "%d", countRecieve);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string(countBuffer, 1);
			nokia_lcd_render();
		}
		
		_delay_ms(250);
    }
}
