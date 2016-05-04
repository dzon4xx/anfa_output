#include "Clock.h"
#include "../../communication/modbus/modbus_slave.h"
#include "../../settings.h"
#include <avr/io.h>
#include <avr/interrupt.h>
//#include "../../settings.h"

#define  OVF_VAL 249	//wartosc rejestru OCR1 aby timer przepelnial sie co 1ms

//Zmienna globalna - szybszy dostep
volatile unsigned long miliseconds_passed = 0;

 Clock::Clock()
{
	cli();
	OCR0A = OVF_VAL;
	TIMSK0 |= (1<<OCIE0A);
	TCCR0A |= (1<WGM01);			
	sei();
}

void Clock::start()
{
	cli();
	TCCR0B |= (1<<CS00)|(1<<CS01);
	sei();
}

void Clock::stop()
{
	cli();
	TCCR0B &= ~(1<<CS00)|(1<<CS01);
	sei();
}

void Clock::restart()
{
	miliseconds_passed = 0;
	this->stop();
	OCR1A = OVF_VAL;
	this->start();
}

unsigned long Clock::get_ms()
{
	return miliseconds_passed;
}


Clock *clock;

ISR (TIMER0_COMPA_vect) 
{
	miliseconds_passed++;
}