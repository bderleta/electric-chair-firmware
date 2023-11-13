/* 
 * File:   prog.h
 * Author: Bartosz Derleta <bartosz@derleta.com>
 */

#ifndef PROG_H
#define	PROG_H

#include "config.h"
#include "avr_database.h"

#define XTAL1_POSITIVE_PULSE() { \
    PORTD.OUTSET = PD_XTAL1_bm; \
    asm("nop"); \
    PORTD.OUTCLR = PD_XTAL1_bm; \
    asm("nop"); \
}

#define WR_NEGATIVE_PULSE() { \
    PORTD.OUTSET = PD_WR_bm; \
    asm("nop"); \
    PORTD.OUTCLR = PD_WR_bm; \
    asm("nop"); \
}

/**
 * https://stackoverflow.com/a/3208376
 */
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


const avr_record* lookup_signature(const uint32_t signature);

void read_signature(uint32_t* signature);
void read_fuse_and_lock_bits(uint8_t* fuse_and_lock_bits, uint8_t read_extended);
void erase_chip();
void program_fuse_low_bits(const uint8_t bits);
void program_fuse_high_bits(const uint8_t bits);
void program_fuse_extended_bits(const uint8_t bits);

void enter_programming();
void exit_programming();
void power_up();
void power_down();

void load_command(const uint8_t command);
void load_address_high_byte(const uint8_t address);
void load_address_low_byte(const uint8_t address);
void load_data_low_byte(const uint8_t data);
void load_data_high_byte(const uint8_t data);
uint8_t read_data(void);
uint8_t read_byte(void);
uint16_t read_word(void);

#endif	/* PROG_H */

