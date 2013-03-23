/*
 * uart.h
 *
 * Hardware dependent
 */

#ifndef UART_H_
#define UART_H_

void UARTPutC( char c );

int UARTGetC();

void UARTInit( unsigned long baud );

#endif /* UART_H_ */
