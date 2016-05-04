/* 
* top_gate.cpp
*
* Created: 2016-03-13 16:08:45
* Author: jan.pleszynski
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "top_gate.h"
#include "../../settings.h"
#include "../../utilities/clock/clock.h"

#define SPI_TIMEOUT 10	//czas w ms na transmisje SPI

Top_gate::Top_gate()
{
	this->init();
} 

bool Top_gate::transmit(volatile uint16_t regs[])
{
	for (uint8_t i=0; i<NUM_OF_CHANNELS; i++)
	{
		uint8_t out_byte;
		regs[i]>0 ?	out_byte=1 : out_byte=0;
		SPDR = out_byte;
		while(!(SPSR)&(1<<SPIF));
	}
	SPDR = '\n';
	while(!(SPSR)&(1<<SPIF));
	return true;
}

void Top_gate::init()
{
	SPCR = ((1<<SPE)|(1<<MSTR)|(1<<SPR0));
	DDRB |= ((1<<PINB3)|(1<<PINB5));
}
