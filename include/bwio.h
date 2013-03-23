/*
 * bwio.h - busy-wait I/O routines
 *
 */

#include <uart.h>

#ifndef  BWIO_H_
#define  BWIO_H_

void bwprintf( char *format, ... );

int sprintf(char *out, const char *format, ...);

#endif /* BWIO_H_ */
