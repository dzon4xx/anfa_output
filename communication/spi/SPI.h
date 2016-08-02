/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 1

// SPI_ATOMIC_VERSION means that SPI has atomicity fixes and what version.
// This way when there is a bug fix you can check this define to alert users
// of your code if it uses better version of this library.
// This also implies everything that SPI_HAS_TRANSACTION as documented above is
// available too.
#define SPI_ATOMIC_VERSION 1

// Uncomment this line to add detection of mismatched begin/end transactions.
// A mismatch occurs if other libraries fail to use SPI.endTransaction() for
// each SPI.beginTransaction().  Connect an LED to this pin.  The LED will turn
// on if any mismatch is ever detected.
//#define SPI_TRANSACTION_MISMATCH_LED 5

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

// define SPI_AVR_EIMSK for AVR boards with external interrupt pins
#if defined(EIMSK)
  #define SPI_AVR_EIMSK  EIMSK
#elif defined(GICR)
  #define SPI_AVR_EIMSK  GICR
#elif defined(GIMSK)
  #define SPI_AVR_EIMSK  GIMSK
#endif




class SPIClass {
public:
  // Initialize the SPI library
  SPIClass();
  
  static void begin();

  // If SPI is used from within an interrupt, this function registers
  // that interrupt with the SPI library, so beginTransaction() can
  // prevent conflicts.  The input interruptNumber is the number used
  // with attachInterrupt.  If SPI is used from a different interrupt
  // (eg, a timer), interruptNumber should be 255.
  static void usingInterrupt(uint8_t interruptNumber);
  // And this does the opposite.
  static void notUsingInterrupt(uint8_t interruptNumber);
  // Note: the usingInterrupt and notUsingInterrupt functions should
  // not to be called from ISR context or inside a transaction.
  // For details see:
  // https://github.com/arduino/Arduino/pull/2381
  // https://github.com/arduino/Arduino/pull/2449

  // Before using SPI.transfer() or asserting chip select pins,
  // this function is used to gain exclusive access to the SPI bus
  // and configure the correct settings.

  // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
  inline static uint8_t transfer(uint8_t data) {
    SPDR = data;
    /*
     * The following NOP introduces a small delay that can prevent the wait
     * loop form iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
    asm volatile("nop");
    while (!(SPSR & _BV(SPIF))) ; // wait
    return SPDR;
  }
  inline static uint16_t transfer16(uint16_t data) {
    union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
    in.val = data;
    if (!(SPCR & _BV(DORD))) {
      SPDR = in.msb;
      asm volatile("nop"); // See transfer(uint8_t) function
      while (!(SPSR & _BV(SPIF))) ;
      out.msb = SPDR;
      SPDR = in.lsb;
      asm volatile("nop");
      while (!(SPSR & _BV(SPIF))) ;
      out.lsb = SPDR;
    } else {
      SPDR = in.lsb;
      asm volatile("nop");
      while (!(SPSR & _BV(SPIF))) ;
      out.lsb = SPDR;
      SPDR = in.msb;
      asm volatile("nop");
      while (!(SPSR & _BV(SPIF))) ;
      out.msb = SPDR;
    }
    return out.val;
  }
  inline static void transfer(void *buf, size_t count) {
    if (count == 0) return;
    uint8_t *p = (uint8_t *)buf;
    SPDR = *p;
    while (--count > 0) {
      uint8_t out = *(p + 1);
      while (!(SPSR & _BV(SPIF))) ;
      uint8_t in = SPDR;
      SPDR = out;
      *p++ = in;
    }
    while (!(SPSR & _BV(SPIF))) ;
    *p = SPDR;
  }
  // After performing a group of transfers and releasing the chip select
  // signal, this function allows others to access the SPI bus

  // Disable the SPI bus
  static void end();

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setBitOrder(uint8_t bitOrder) {
    if (bitOrder == LSBFIRST) SPCR |= _BV(DORD);
    else SPCR &= ~(_BV(DORD));
  }
  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setDataMode(uint8_t dataMode) {
    SPCR = (SPCR & ~SPI_MODE_MASK) | dataMode;
  }
  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setClockDivider(uint8_t clockDiv) {
    SPCR = (SPCR & ~SPI_CLOCK_MASK) | (clockDiv & SPI_CLOCK_MASK);
    SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((clockDiv >> 2) & SPI_2XCLOCK_MASK);
  }
  // These undocumented functions should not be used.  SPI.transfer()
  // polls the hardware flag which is automatically cleared as the
  // AVR responds to SPI's interrupt
  inline static void attachInterrupt() { SPCR |= _BV(SPIE); }
  inline static void detachInterrupt() { SPCR &= ~_BV(SPIE); }

private:
  static uint8_t initialized;
  static uint8_t interruptMode; // 0=none, 1=mask, 2=global
  static uint8_t interruptMask; // which interrupts to mask
  static uint8_t interruptSave; // temp storage, to restore state
};

extern SPIClass *SPI;

#endif