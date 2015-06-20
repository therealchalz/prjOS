/*
 * bwio.h
 *
 *  Created on: Dec 24, 2014
 *      Author: che
 */

#ifndef BWIO_H_
#define BWIO_H_

#include <include/bwio.h>

int bwprintf(const char *format, ...);

int bwprintf2(const char *format, int* varg);

int bwsprintf(char *out, const char *format, ...);

int bwsprintf2(char *out, const char *format, int* varg);

void bwputc(int out);

#endif /* BWIO_H_ */
