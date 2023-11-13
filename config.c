#include "config.h"

static FILE usart0_out_stream = FDEV_SETUP_STREAM(usart1_putc, NULL, _FDEV_SETUP_WRITE);

int usart1_gets(char* buf, const unsigned int buf_size) {
    unsigned int read = 0;
    if (buf_size < 1) {
        return -1;
    }
    while (read < buf_size - 1) {
        char data = usart1_getc(NULL);
        if (data == '\r') {
            usart1_putc('\n', NULL);
            break;
        } else if (
            ((data >= 'a') && (data <= 'z')) ||
            ((data >= 'A') && (data <= 'Z')) ||
            ((data >= '0') && (data <= '9')) ||
            (data == ' ')
        ) {
            buf[read++] = data;
            usart1_putc(data, NULL);
        } else if ((data == 0x08) && (read > 0)) {
            usart1_puts("\x08\x20\x08");
            buf[--read] = 0;
        }
    }
    buf[read] = 0;
    return read;
}

/** PORTA
 * 0 D0
 * 1 D1
 * 2 D2
 * 3 D3
 * 4 D4
 * 5 D5
 * 6 D6
 * 7 D7
 */
void porta_init(void) {
    PORTA.DIRCLR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | 
            PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
    PORTA.OUTCLR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | 
            PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
}

/** PORTC
 * 0 TXD1 (USB_UART)
 * 1 RXD1 (USB_UART)
 * 2 SDA                    I2C data
 * 3 SCL                    I2C clock
 */
void portc_init(void) {
    PORTC.DIRSET = PIN0_bm | PIN2_bm | PIN3_bm;
    PORTC.DIRCLR = PIN1_bm;
    PORTC.OUTCLR = PIN0_bm | PIN2_bm | PIN3_bm; // silicon errata 2.15.1
}

/** PORTD
 * 1 PAGEL
 * 2 XA1
 * 3 XA0
 * 4 BS1
 * 5 ~{WR}
 * 6 ~{OE}
 * 7 XTAL1
 */
void portd_init(void) {
    PORTD.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc; // silicon errata 2.9.1
    PORTD.PIN5CTRL = PORT_INVEN_bm;
    PORTD.PIN6CTRL = PORT_INVEN_bm;
    PORTD.DIRCLR = PD_PAGEL_bm | PD_XA1_bm | PD_XA0_bm | 
            PD_BS1_bm | PD_WR_bm | PD_OE_bm | PD_XTAL1_bm;
    PORTD.OUTCLR = PD_PAGEL_bm | PD_XA1_bm | PD_XA0_bm | 
            PD_BS1_bm | PD_WR_bm | PD_OE_bm | PD_XTAL1_bm;
}

void portd_control(void) {
    PORTD.DIRSET = PD_PAGEL_bm | PD_XA1_bm | PD_XA0_bm | 
        PD_BS1_bm | PD_WR_bm | PD_OE_bm | PD_XTAL1_bm;
    PORTD.OUTCLR = PD_PAGEL_bm | PD_XA1_bm | PD_XA0_bm | 
        PD_BS1_bm | PD_WR_bm | PD_OE_bm | PD_XTAL1_bm;
}

/** PORTF
 * 0 BS2
 * 1 TRESET                 Target reset pull-down
 * 2 12V_EN                 Target reset 12V pull-up
 * 3 5V_EN                  Target power
 * 4 RDY_BSY                RDY/~{BSY} signal
 * 5 ~{PROGRAM}             "Program" button
 * 6 ~{RESET}               (Hardware)
 */
void portf_init(void) {
    PORTF.PIN5CTRL = PORT_INVEN_bm;
    PORTF.DIRSET = PF_TRESET_bm | PF_12V_EN_bm | PF_5V_EN_bm;
    PORTF.DIRCLR = PF_BS2_bm | PF_PROGRAM_bm | PF_RDY_BSY_bm;
    PORTF.OUTCLR = PF_BS2_bm | PF_TRESET_bm | PF_12V_EN_bm | PF_5V_EN_bm;
}

/*
 
 */
void portf_control(void) {
    PORTF.DIRSET = PF_BS2_bm;
    PORTF.OUTCLR = PF_BS2_bm;
}

/** USART1                  Debug UART
 * - Baud rate USART0_BAUDRATE, 8 data bit, 1 stop bit, no parity
 * - MSb of the data word is transmitted first
 * - Data are sampled on the leading (first) edge
 * - Normal-Speed mode
 * - CTRLA: No interrupts enabled
 */
void usart1_init(void) {
    /* S is the number of samples per bit
       - Asynchronous Normal mode: S = 16
       - Asynchronous Double-Speed mode: S = 8
       - Synchronous mode: S = 2 
     */
    PORTMUX.USARTROUTEA = PORTMUX_USART0_NONE_gc | PORTMUX_USART1_DEFAULT_gc | 
            PORTMUX_USART2_NONE_gc;
    USART1.BAUD = (64.0 * F_CPU) / (/* S */ 16.0 * USART1_BAUDRATE);
    // USART1.CTRLA = USART_RXCIE_bm; 
    USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_SBMODE_1BIT_gc;
    USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
    stdout = &usart0_out_stream;
}

int usart1_putc(const char c, FILE *stream) {
    if (c == '\n') {
        usart1_putc('\r', stream);
    }
	while (!(USART1.STATUS & USART_DREIF_bm));
	USART1.TXDATAL = c;
    return 0;
}

void usart1_puts(const char* s) {
    while (*s) {
        usart1_putc(*(s++), NULL);
    }
}

int usart1_getc(FILE* stream) {
    while (!(USART1.STATUS & USART_RXCIF_bm));
    return USART1.RXDATAL;
}

void init(void) {
    /** Disable prescaler */
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x00);
    porta_init();
    portc_init();
    portd_init();
    portf_init();
    usart1_init();
    sei();
}