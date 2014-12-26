/*
 * bwio.c
 *
 *  Created on: Dec 24, 2014
 *      Author: che
 */


#include "prjOS/include/bwio.h"
#include "include/bwio.h"

extern int eprintf2(const char *format, int* varg);
extern int esprintf2(char *out, const char *format, int* varg);

int bwprintf(const char *format, ...) {
	//register int *varg = (int *)(&format);
	//return eprintf2(format, varg);
	return 0;
}

int bwsprintf(char *out, const char *format) {
	//register int *varg = (int *)(&format);
	//return esprintf2(out, format, varg);
	return 0;
}
