#include "prog.h"

void read_signature(uint32_t* signature) {
    uint8_t* sig = (uint8_t*) signature;
    load_command(0b00001000);
    sig[3] = 0x00;
    load_address_low_byte(0x00);
    sig[2] = read_byte();
    load_address_low_byte(0x01);
    sig[1] = read_byte();
    load_address_low_byte(0x02);
    sig[0] = read_byte();
}

const avr_record* lookup_signature(const uint32_t signature) {
    const int database_length = sizeof(database) / sizeof(avr_record);
    for (int i = 0; i < database_length; i++) {
        if (database[i].signature == signature) {
            return &database[i];
        }
    }
    return NULL;
}

void read_fuse_and_lock_bits(uint8_t* fuse_and_lock_bits, uint8_t read_extended) {
    load_command(0b00000100);
    PORTF.OUTCLR = PF_BS2_bm;
    PORTD.OUTCLR = PD_BS1_bm;
    fuse_and_lock_bits[0] = read_data(); // Fuse Low
    PORTF.OUTSET = PF_BS2_bm;
    PORTD.OUTSET = PD_BS1_bm;
    fuse_and_lock_bits[1] = read_data(); // Fuse High
    if (read_extended) {
        PORTF.OUTSET = PF_BS2_bm;
        PORTD.OUTCLR = PD_BS1_bm;
        fuse_and_lock_bits[2] = read_data(); // Fuse Ext
    } else {
        fuse_and_lock_bits[2] = 0xFF; // even if you try to read it unsupported, it is 0xFF
    }
    PORTF.OUTCLR = PF_BS2_bm;
    PORTD.OUTSET = PD_BS1_bm;
    fuse_and_lock_bits[3] = read_data(); // Lock 
}

void read_flash(const uint16_t address, uint16_t* data) {
    load_command(0b00000010);
    load_address_high_byte(address >> 8);
    load_address_low_byte(address & 0xFF);
    *data = read_word();
}

void program_fuse_low_bits(const uint8_t bits) {
    // A: Load Command “0100 0000”
    load_command(0b01000000);
    // C: Load Data Low Byte. Bit n = “0” programs and bit n = “1” erases the Fuse bit.
    load_data_low_byte(bits);
    // Set BS1 to “0” and BS2 to “0”.
    PORTD.OUTCLR = PD_BS1_bm;
    PORTF.OUTCLR = PF_BS2_bm;
    // Give WR a negative pulse and wait for RDY/BSY to go high.
    WR_NEGATIVE_PULSE();
    while (PORTF.IN & PF_RDY_BSY_bm);
    while (!(PORTF.IN & PF_RDY_BSY_bm));
}

void program_fuse_high_bits(const uint8_t bits) {
    // A: Load Command “0100 0000”
    load_command(0b01000000);
    // C: Load Data Low Byte. Bit n = “0” programs and bit n = “1” erases the Fuse bit.
    load_data_low_byte(bits);
    // Set BS1 to “1” and BS2 to “0”. This selects high data byte.
    PORTD.OUTSET = PD_BS1_bm;
    PORTF.OUTCLR = PF_BS2_bm;
    // Give WR a negative pulse and wait for RDY/BSY to go high.
    WR_NEGATIVE_PULSE();
    while (PORTF.IN & PF_RDY_BSY_bm);
    while (!(PORTF.IN & PF_RDY_BSY_bm));    
    // Set BS1 to “0”. This selects low data byte.
    PORTD.OUTCLR = PD_BS1_bm;
}

void program_fuse_extended_bits(const uint8_t bits) {
    // A: Load Command “0100 0000”
    load_command(0b01000000);
    // C: Load Data Low Byte. Bit n = “0” programs and bit n = “1” erases the Fuse bit.
    load_data_low_byte(bits);
    // Set BS2, BS1 to “10”. This selects extended data byte.
    PORTF.OUTSET = PF_BS2_bm;
    PORTD.OUTCLR = PD_BS1_bm;
    // Give WR a negative pulse and wait for RDY/BSY to go high.
    WR_NEGATIVE_PULSE();
    while (PORTF.IN & PF_RDY_BSY_bm);
    while (!(PORTF.IN & PF_RDY_BSY_bm));    
    // Set BS2, BS1 to “00”. This selects low data byte.
    PORTF.OUTCLR = PF_BS2_bm;
}

void erase_chip() {
    // Set XA1, XA0 to “10”. This enables command loading.
    // Set BS1 to “0”.
    // Set DATA to “1000 0000”. This is the command for Chip Erase.
    // Give XTAL1 a positive pulse. This loads the command.
    load_command(0b10000000);
    // Give WR a negative pulse. This starts the Chip Erase. RDY/BSY goes low.
    WR_NEGATIVE_PULSE();
    while (PORTF.IN & PF_RDY_BSY_bm);
    // Wait until RDY/BSY goes high before loading a new command.
    while (!(PORTF.IN & PF_RDY_BSY_bm));
}

/* Enter/exit programming mode */

void enter_programming() {
    // Apply 5V and wait at least 100 μs
    PORTF.OUTSET = PF_5V_EN_bm | PF_TRESET_bm;
    _delay_us(200);
    
    // Set RESET to 0 and toggle XTAL1 at least 6 times 
    portd_control(); // Enable driving control signals
    portf_control();
    
    _delay_us(1);
    for (int i = 0; i < 6; i++) {
        XTAL1_POSITIVE_PULSE();
    }
    
    // Set the Prog_enable pins to \"0000\" and wait at least 100 ns
    PORTD.OUTCLR = PD_PAGEL_bm | PD_XA1_bm | PD_XA0_bm | PD_BS1_bm;
    PORTD.OUTSET = PD_WR_bm;
    _delay_us(1);
    
    // Apply 12V to RESET 
    PORTF.OUTCLR = PF_TRESET_bm;
    PORTF.OUTSET = PF_12V_EN_bm;
    _delay_us(50);
    PORTD.OUTCLR = PD_WR_bm;
}

void power_up() {
    PORTF.OUTSET = PF_5V_EN_bm | PF_TRESET_bm;
    PORTF.DIRCLR = PF_RDY_BSY_bm; // // RDY/~{BSY} hi-z
    _delay_ms(1);
    PORTF.OUTCLR = PF_TRESET_bm;
}

void power_down() {
    PORTF.DIRSET = PF_RDY_BSY_bm; // // RDY/~{BSY} pulldown
    PORTF.OUTCLR = PF_5V_EN_bm | PF_RDY_BSY_bm;
}

void exit_programming() {
    PORTF.OUTCLR = PF_12V_EN_bm;
    PORTF.OUTSET = PF_TRESET_bm;
    _delay_ms(1);
    // Reset all signals to Hi-Z, disable driving control signals
    // It seems that ATmega chips can backfeed power from GPIO lines
    // through their protection diodes, and this is violating Absolute Maximum
    // Ratings for the chip
    porta_init(); // Data lines
    portd_init(); // XA0, XA1, BS1, OE, WR, XTAL1, RDY/BSY
    portf_init(); // BS2
    /* Power down and reset release is implied by portf reinit */
}

/* Low-level programming commands */

void load_command(const uint8_t command) {
    // Set XA1, XA0 to “10”. This enables command loading.
    PORTD.OUTSET = PD_XA1_bm;
    PORTD.OUTCLR = PD_XA0_bm;
    // Set BS1 to “0”.
    PORTD.OUTCLR = PD_BS1_bm;
    // Set DATA to <command>.
    PORTA.DIRSET = 0xFF;
    PORTA.OUT = command;
    // Give XTAL1 a positive pulse. This loads the command.
    XTAL1_POSITIVE_PULSE();
    PORTA.OUT = 0x00;
}

void load_address_low_byte(const uint8_t address) {
    // Set XA1, XA0 to “00”. This enables address loading.
    PORTD.OUTCLR = PD_XA1_bm | PD_XA0_bm;
    // Set BS1 to “0”. This selects low address.
    PORTD.OUTCLR = PD_BS1_bm;
    // Set DATA = Address low byte ($00 - $FF).
    PORTA.DIRSET = 0xFF;
    PORTA.OUT = address;
    // Give XTAL1 a positive pulse. This loads the address low byte.
    XTAL1_POSITIVE_PULSE();
    PORTA.OUT = 0x00;
}

void load_address_high_byte(const uint8_t address) {
    // Set XA1, XA0 to “00”. This enables address loading.
    PORTD.OUTCLR = PD_XA1_bm | PD_XA0_bm;
    // Set BS1 to “1”. This selects high address.
    PORTD.OUTSET = PD_BS1_bm;
    // Set DATA = Address high byte ($00 - $FF).
    PORTA.DIRSET = 0xFF;
    PORTA.OUT = address;
    // Give XTAL1 a positive pulse. This loads the address low byte.
    XTAL1_POSITIVE_PULSE();
    PORTA.OUT = 0x00;
}

void load_data_low_byte(const uint8_t data) {
    // Set XA1, XA0 to “01”. This enables data loading.
    PORTD.OUTCLR = PD_XA1_bm;
    PORTD.OUTSET = PD_XA0_bm;
    // Set BS1 to “0”.
    PORTD.OUTCLR = PD_BS1_bm;
    // Set DATA = Data low byte ($00 - $FF).
    PORTA.DIRSET = 0xFF;
    PORTA.OUT = data;
    // Give XTAL1 a positive pulse. This loads the data byte.
    XTAL1_POSITIVE_PULSE();
    PORTA.OUT = 0x00;
}

void load_data_high_byte(const uint8_t data) {
    // Set BS1 to “1”. This selects high data byte.
    PORTD.OUTSET = PD_BS1_bm;
    // Set XA1, XA0 to “01”. This enables data loading.
    PORTD.OUTCLR = PD_XA1_bm;
    PORTD.OUTSET = PD_XA0_bm;
    // Set DATA = Data high byte ($00 - $FF).
    PORTA.DIRSET = 0xFF;
    PORTA.OUT = data;
    // Give XTAL1 a positive pulse. This loads the data byte.
    XTAL1_POSITIVE_PULSE();
    PORTA.OUT = 0x00;    
}

uint8_t read_data(void) {
    PORTA.DIRCLR = 0xFF;
    // Set OE to “0”. The <data> byte can now be read at DATA.
    PORTD.OUTSET = PD_OE_bm; // INVEN is enabled on this output
    _delay_us(1);
    uint8_t data = PORTA.IN;
    PORTD.OUTCLR = PD_OE_bm;
    return data;
}

uint8_t read_byte(void) {
    PORTA.DIRCLR = 0xFF;
    // Set OE to “0”, and BS1 to “0”. The <data> byte can now be read at DATA.
    PORTD.OUTCLR = PD_BS1_bm;
    PORTD.OUTSET = PD_OE_bm; // INVEN is enabled on this output
    _delay_us(1);
    uint8_t data = PORTA.IN;
    PORTD.OUTCLR = PD_OE_bm;
    return data;
}

uint16_t read_word(void) {
    PORTA.DIRCLR = 0xFF;
    // Set OE to “0”, and BS1 to “0”. The <data> byte can now be read at DATA.
    PORTD.OUTCLR = PD_BS1_bm;
    PORTD.OUTSET = PD_OE_bm; // INVEN is enabled on this output
    _delay_us(1);
    uint8_t low = PORTA.IN;
    PORTD.OUTSET = PD_BS1_bm;
    uint8_t high = PORTA.IN;
    PORTD.OUTCLR = PD_OE_bm | PD_BS1_bm;
    return low | (high << 8);
}