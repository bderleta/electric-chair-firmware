/* 
 * File:   config.h
 * Author: Bartosz Derleta <bartosz@derleta.com>
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <ctype.h>

/* Debug UART full duplex */
#define USART1_BAUDRATE 115200
    
void init(void);

void porta_init(void);
void portd_init(void);
void portd_control(void);
void portf_init(void);
void portf_control(void);

int usart1_putc(const char c, FILE *stream);
void usart1_puts(const char* s);
int usart1_getc(FILE* stream);
int usart1_gets(char* buf, const unsigned int buf_size);

#define PF_BS2_bm PIN0_bm
#define PF_TRESET_bm PIN1_bm
#define PF_12V_EN_bm PIN2_bm
#define PF_5V_EN_bm PIN3_bm
#define PF_RDY_BSY_bm PIN4_bm
#define PF_PROGRAM_bm PIN5_bm

#define PD_PAGEL_bm PIN1_bm
#define PD_XA1_bm PIN2_bm
#define PD_XA0_bm PIN3_bm
#define PD_BS1_bm PIN4_bm
#define PD_WR_bm PIN5_bm
#define PD_OE_bm PIN6_bm
#define PD_XTAL1_bm PIN7_bm

#endif	/* CONFIG_H */

