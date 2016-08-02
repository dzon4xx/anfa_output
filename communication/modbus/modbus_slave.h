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
#define MAX_FIXED_BYTES_IN_REQUEST 9
#define MAX_REGISTERS 64

 #define DEFAULT_MODBUS_ADDRESS 2
 //#define BAUDRATE 19200L


//Transmision buffer size
#define BUFFER_SIZE 2*MAX_REGISTERS+MAX_FIXED_BYTES_IN_REQUEST//maks 256 bajtow do komunikacji

class Frame
{
public:

	Frame();

	//Czyta przychodzace bajty. sprawdza czy przyszla nowa ramka
	uint8_t read_byte();

	//Sprawdza dlugosc przychodzacej ramki
	void check_length(uint8_t in_byte);

	void listen_mode();

	void send_mode(uint8_t response_frame_size);

	// Odpowiedz bledu.
	void exception_response(uint8_t function, uint8_t exception);

	//Wyliczanie sumy kontrolnej
	uint16_t calculate_CRC(uint8_t size);

public:

	//Adres sluzacy do komunikacji modbus
	uint8_t address = DEFAULT_MODBUS_ADDRESS;

	//Licznik przychodzacych bajtow
	uint8_t in_bytes_counter = 0;

	//Flaga mowiaca czy przyszla nowa ramka danych.
	bool is_new = false;

	//Dlugosc przychodzacej ramki
	uint16_t length = 65535;

	//Bufor sluzacy do odczytywania i wysylania danych przez UART
	uint8_t data[BUFFER_SIZE] = {0};

	volatile uint8_t send_data_pointer = 0;
	
	volatile uint8_t send_data_size = 0;

	volatile uint8_t *uart_port = &UDR0;

private:

	//Konfiguracja komunikacji szeregowej
	void init_UART();

	//Wylicz czasy opoznien do T1_5 oraz T3_5
	void calculate_time_outs(uint32_t baude_rate);

private:

	Out_pin *pin;
	
	long last_byte_arrival_t = 0;

	uint8_t T3_5_ms = 1; // frame delay in ms

	unsigned int T3_5_us = 1000; // frame delay in us
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

	void populate_regs_with_frame(uint8_t data[], uint8_t no_of_coils);

	//Nowy pakiet czekajacy na osbluzenie
	volatile bool new_packet_pending = false;
};

class Modbus_write_coil : public Modbus_write
{
public:
	Modbus_write_coil();

	//Zapisuje rejestry modbus
	void write_coils();

private:
};

extern Modbus_write_coil *write_coil1;
extern Frame *frame;


#endif //MODBUS_SLAVE_H_


