/*
 * pin.h
 *
 * Created: 2016-03-03 10:58:58
 *  Author: jan.pleszynski
 */ 


#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

class Pin
{
	public:

	//zwraca litere portu
	uint8_t get_port_letter();

	//zwraca maske pinu. 
	uint8_t get_pin_mask();

protected:

	//Maska pinu
	uint8_t pin_mask;

	uint8_t port_letter;

};

class Out_pin : public Pin
{
public:

	Out_pin(uint8_t port_letter, uint8_t pin_num);
	volatile uint8_t* get_out_port();

	//Ustawia pin w stan wysoki
	void high();

	//Ustawia pin w stan wysoki
	void low();
private:

	//Rejestr wyjsciowy PORTX
	volatile uint8_t *out_port;

};
#endif