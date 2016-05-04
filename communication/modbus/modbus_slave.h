#ifndef MODBUS_SLAVE_H_
#define MODBUS_SLAVE_H_

/*
 * modbusgate.h
 *
 *  Created on: 12 lut 2016
 *      Author: jan.pleszynski
 */


#include "../../utilities/io/pin/pin.h"
#include "../../settings.h"
#include <stdint.h>
#include <avr/io.h>
#define MAX_FIXED_BYES_IN_REQUEST 9
#define MAX_REGISTERS 10

 #define DEFAULT_MODBUS_ADDRESS 11
 #define DEFAULT_BAUDE_RATE 57600


//Transmision buffer size
#define BUFFER_SIZE 2*MAX_REGISTERS+MAX_FIXED_BYES_IN_REQUEST//maks 256 bajtow do komunikacji

class Frame
{
public:

	Frame(uint8_t port_letter, uint8_t pin_num);

	//Czyta przychodzace bajty. sprawdza czy przyszla nowa ramka
	uint8_t read_byte();

	//Sprawdza dlugosc przychodzacej ramki
	void check_length(uint8_t in_byte);

	//Wyslij ramke poprzez modbus
	void send(unsigned char bufferSize);

	// Odpowiedz bledu.
	void exception_response(uint8_t function, uint8_t exception);

	//Wyliczanie sumy kontrolnej
	uint16_t calculate_CRC(uint8_t size);

public:

	//Adres sluzacy do komunikacji modbus
	uint8_t address = DEFAULT_MODBUS_ADDRESS;

	//Predkosc transmisji
	unsigned long baude_rate = DEFAULT_BAUDE_RATE;

	//Licznik przychodzacych bajtow
	uint8_t in_bytes_counter = 0;

	//Flaga mowiaca czy przyszla nowa ramka danych.
	bool is_new = false;

	//flaga mowiaca o tym czy czytac ramke. Czytanie ramki jest wtedy gdy zapytanie przyszlo pod dobry adres
	bool read_data = false;

	//Dlugosc przychodzacej ramki
	uint16_t length = -1;

	//Bufor sluzacy do odczytywania i wysylania danych przez UART
	uint8_t data[BUFFER_SIZE] = {0};

private:

	//Konfiguracja komunikacji szeregowej
	void set_UART();

	void listen_mode();

	void send_mode();

	//Wylicz czasy opoznien do T1_5 oraz T3_5
	void calculate_time_outs(uint32_t baude_rate);

	unsigned int T1_5 = 750; // inter character time out

	unsigned int T3_5 = 1750; // frame delay

private:

	#ifdef ATMEGA32U4
	volatile uint8_t *udr_port = &UDR1;
	#endif

	#ifdef ATMEGA328
	volatile uint8_t *udr_port = &UDR0;
	#endif

	Out_pin *pin;
	
	long last_byte_arrival_t = 0;
};

class Modbus_base
{
	public:

	//Rejestry modbusowe
	volatile uint16_t regs[MAX_REGISTERS] = {0};
};

class Modbus_write : public Modbus_base
{
public:


	//Nowy pakiet czekajacy na osbluzenie
	volatile bool new_packet_pending = true;
};

class Modbus_write_coil : public Modbus_write
{
public:
	Modbus_write_coil();

	void populate_regs_with_frame(uint8_t frame[], uint8_t no_of_registers);


	//Zapisuje rejestry modbus
	void write_coils();

private:
};

extern Modbus_write_coil *write_coil1;
extern Frame *frame;


#endif //MODBUS_SLAVE_H_


