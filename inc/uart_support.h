/*
 UART2TX : PA2
 UART2RX : PA3
 UART3TX : PD8
 UART3RX : PD9
 UART4TX : PA0
 UART4RX : PA1

 UART3 は GPS用
*/



#ifndef __UART_SUPPORT_H
#define __UART_SUPPORT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* General Inclusion */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/* uC Specific Inclusion */
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_conf.h"

/* USART Definition */
#define UART_BUFSIZE		1024
#define UART_BAUDLATE		38400UL
#define UART_DEFAULT_NUM	1 /*USART2 PA2,PA3 default use*/

/* General Definition */
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Funcion Prototypes */
extern void conio_init(uint32_t port, uint32_t baudrate);
extern void set_defalt_cino_port(uint32_t port);
extern void putch(uint8_t c);
extern void putch_p(uint32_t port,uint8_t c);
extern uint8_t getch(void);
extern uint8_t getch_p(uint32_t port);
extern uint16_t getch_p16(uint32_t port);
extern uint8_t keypressed(uint32_t port);
extern void cputs(char *s);
extern void cgets(uint32_t port,char *s, int bufsize);

/* Structs */
/* This is From AVRX uC Sample!!! */
/* @brief USART transmit and receive ring buffer. */
typedef struct USART_Buffer
{
	/* @brief Receive buffer. */
	volatile uint8_t RX[UART_BUFSIZE];
	/* @brief Transmit buffer. */
	volatile uint8_t TX[UART_BUFSIZE];
	/* @brief Receive buffer head. */
	volatile uint16_t RX_Head;
	/* @brief Receive buffer tail. */
	volatile uint16_t RX_Tail;
	/* @brief Transmit buffer head. */
	volatile uint16_t TX_Head;
	/* @brief Transmit buffer tail. */
	volatile uint16_t TX_Tail;
} USART_Buffer_t;

/* Externs */
extern USART_Buffer_t USART1_Buf;
extern USART_Buffer_t USART2_Buf;
extern USART_Buffer_t USART3_Buf;
extern USART_Buffer_t UART4_Buf;

#ifdef __cplusplus
}
#endif

#endif	/* __UART_SUPPORT_H */
