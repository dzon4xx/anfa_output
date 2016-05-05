/*
 * switch.cpp
 *
 * Created: 2016-03-06 15:11:29
 * Author : jan.pleszynski
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "settings.h"
#include "utilities/io/pin/pin.h"
#include "utilities/clock/clock.h"
#include "communication/modbus/modbus_slave.h"
#include "communication/spi/SPI.h"

#define SPI_START_COM 2
#define ACK 1

int main(void)
{
	cli();
	Out_pin rel1('d', 5);
	Out_pin rel2('d', 6);
	Out_pin rel3('d', 7);
	Out_pin rel4('b', 0);
	Out_pin rel5('b', 1);
	Out_pin rel6('b', 2);
	Out_pin rel7('c', 3);
	Out_pin rel8('c', 2);
	Out_pin rel9('c', 1);
	Out_pin rel10('c', 0);
	Out_pin *rels[NUM_OF_CHANNELS] = {&rel1, &rel2, &rel3, &rel4, &rel5, &rel6, &rel7, &rel8, &rel9, &rel10};

	frame = new Frame('d', 2);
	write_coil1 = new Modbus_write_coil;
	SPI = new SPIClass;
	clock = new Clock;
	clock->start();
	sei();
    while (1) 
    {
		if(write_coil1->new_packet_pending)
		{
			write_coil1->new_packet_pending = false;
			for (uint8_t rel_num=0; rel_num < NUM_OF_CHANNELS; rel_num++)
			{				
				Out_pin *crnt_rel = rels[rel_num];
				if (write_coil1->regs[rel_num] == 0)
				{
					crnt_rel->low();
				} 
				else
				{
					crnt_rel->high();
				}				
			}
			uint8_t top_response = SPI->transfer(SPI_START_COM);
			if (top_response == ACK)
			{
				for (uint8_t rel_num=0; rel_num < NUM_OF_CHANNELS; rel_num++)
				{
					if (write_coil1->regs[rel_num] == 0)
					{
						SPI->transfer(1);
					}
					else
					{
						SPI->transfer(0);
					}
				}
			}
			
		}
    }
}

