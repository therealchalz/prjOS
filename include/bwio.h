/*
 * bwio.h - busy-wait I/O routines for diagnosis
 *
 * Adapted from code given to students for the Fall 2009 Semester
 * of CS452 at the University of Waterloo.
 */

#include <uart.h>

#ifndef BWIO_H_
#define BWIO_H_

typedef char *va_list;

#define __va_argsiz(t)	\
		(((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, pN) ((ap) = ((va_list) __builtin_next_arg(pN)))

#define va_end(ap)	((void)0)

#define va_arg(ap, t)	\
		 (((ap) = (ap) + __va_argsiz(t)), *((t*) (void*) ((ap) - __va_argsiz(t))))

void bwputx( char c );

void bwputstr( char *str );

void bwputr( unsigned int reg );

void bwputw( int n, char fc, char *bf );

void bwprintf( char *format, ... );

int bwa2d( char ch );

#endif /* BWIO_H_ */
