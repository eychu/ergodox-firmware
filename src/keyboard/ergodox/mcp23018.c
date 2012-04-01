/* ----------------------------------------------------------------------------
 * ergoDOX controller: MCP23018 specific code
 * ----------------------------------------------------------------------------
 * Copyright (c) 2012 Ben Blazak <benblazak.dev@gmail.com>
 * Released under The MIT License (MIT) (see "license.md")
 * Project located at <https://github.com/benblazak/ergodox-firmware>
 * ------------------------------------------------------------------------- */

// TODO: this is not working yet

	// see if the device is ready
	// - success: set `mcp23018_ready = true` and continue initializing
	// - failure: return `error`; we can try again later

	// set pin direction
	// - unused  : input  : 1
	// - rows    : output : 0
	// - columns : input  : 1

	// set pull-up
	// - unused  : on : 1
	// - rows    : on : 1
	// - columns : on : 1

	// set output pins high
	// - rows  : high : 1
	// - other : low  : 0 (or ignored)


#include <util/twi.h>

#define MCP23018_h_INCLUDE_PRIVATE
#include "mcp23018.h"
#include "teensy-2-0.h"
#include "lib/data-types.h"


// register addresses (see "mcp23018.md")
#define IODIRA 0x00  // i/o direction register
#define IODIRB 0x01
#define GPPUA  0x0C  // GPIO pull-up resistor register
#define GPPUB  0x0D
#define GPIOA  0x12  // general purpose i/o port register
#define GPIOB  0x13
#define OLATA  0x14  // output latch register
#define OLATB  0x15


// ----------------------------------------------------------------------------
// dbg
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <util/delay.h>

#define blink_led(time1, time2) { \
	/* Teensy 2.0 onboard LED on PD6
	   on high, off low */ \
	PORTD |=  (1<<6); \
	_delay_ms(time1); \
	PORTD &= ~(1<<6); \
	_delay_ms(time2); \
}

void blink_hex(uint8_t num) {
	// initial blink (get ready)
	blink_led(700, 200);
	// 1st hex number
	for (uint8_t i=0; i<(num/0x10); i++) {
		blink_led(200, 100);
	}
	_delay_ms(400);
	// 2nd hex number
	for (uint8_t i=0; i<(num%0x10); i++) {
		blink_led(200, 100);
	}
}

// ---------------------------------------------------------------------------- // TWI
// ----------------------------------------------------------------------------

void twi_init(void) {
	TWSR &= ~( (1<<TWPS1)|(1<<TWPS0) );
	TWBR = ((F_CPU / 400000) - 16) / 2;
// 	TWSR |= (1<<TWPS1)|(1<<TWPS0); //dbg
// 	TWBR = 0xFF; //dbg
}

uint8_t twi_start(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_START)
		return TWSR & 0xF8;  // error
}

void twi_stop(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while (TWCR & (1<<TWSTO));
}

uint8_t twi_send(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
		return TWSR & 0xF8;  // error
}

// ----------------------------------------------------------------------------
// init function
// ----------------------------------------------------------------------------

uint8_t mcp23018_init(void) {
	uint8_t ret;

	twi_init();

	twi_start();
	ret = twi_send( (MCP23018_TWI_ADDRESS<<1) | TW_WRITE );
	ret = twi_send(IODIRA);
	ret = twi_send(0);
	ret = twi_send(0);
	ret = twi_send(0);

	blink_hex(ret);

}
