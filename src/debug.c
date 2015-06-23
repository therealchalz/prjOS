/***************************************
 * Copyright (c) 2013 Charles Hache <chache@brood.ca>. All rights reserved. 
 * 
 * This file is part of the prjOS project.
 * prjOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * prjOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with prjOS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Contributors:
 *     Charles Hache <chache@brood.ca> - initial API and implementation
***************************************/

/*
 * debug.c
 */

#include <prjOS/include/debug.h>

// byteGroupSize={1,2,4}
// showAsLittleEndian={0,!0}

void prettyPrintMemory(char *location, uint16_t numBytes, uint8_t byteGroupSize, uint8_t showAsLittleEndian) {
	if (numBytes == 0)
		return;
	//Word aligned print
	uint16_t bytesToSkip = ((uint32_t)location % 16);
	location -= bytesToSkip;

	numBytes += bytesToSkip;
	uint16_t bytesLeft = numBytes;

	while (bytesLeft > 0) {
		bwprintf("%08x", location);
		uint8_t bytesThisLine = 16;
		if (bytesLeft < bytesThisLine)
			bytesThisLine = bytesLeft;
		char groupSize = 0;
		uint8_t b;
		for (b = 0; b<bytesThisLine; b++) {
			if (groupSize >= byteGroupSize)
				groupSize = 0;

			if (bytesToSkip > 0) {
				bytesToSkip--;

				if (groupSize == 0)
					bwprintf("   ", *location++);
				else
					bwprintf("  ", *location++);
			} else {
				char byteToWrite;

				if (showAsLittleEndian) {
					byteToWrite = *(location+byteGroupSize-groupSize-1);
					if (groupSize == byteGroupSize-1)
						location+=byteGroupSize;
				} else {
					byteToWrite = *location++;
				}

				if (groupSize == 0) {
					bwprintf(" %02x", byteToWrite);
				} else {
					bwprintf("%02x", byteToWrite);
				}
			}

			groupSize++;
			bytesLeft--;
		}
		bwprintf("\r\n");
	}
}
void prettyPrintMemoryBytes(char *location, uint16_t numBytes) {
	prettyPrintMemory(location, numBytes, 1, 0);

}
void prettyPrintMemory32Bit(char* location, uint16_t numWords) {
	prettyPrintMemory(location, numWords*4, 4, 1);
}
void printStackTop(char* stackLocation, uint16_t numItems) {
	prettyPrintMemory32Bit(stackLocation, numItems);
}
void printCurrentStackTop(uint16_t numItems) {
	uint32_t x = 4;
	uint32_t *px = &x;
	uint32_t skipAmount = 5;
	printStackTop((char*)(px+skipAmount), numItems);
}
void printCEnvironmentSettings() {
	bwprintf("Current Environment: \r\n");
	bwprintf("Size of int: %d\r\n", sizeof(int));
	bwprintf("Size of long: %d\r\n", sizeof(long));
	bwprintf("Size of long long: %d\r\n", sizeof (long long));
	bwprintf("Size of float: %d\r\n", sizeof(float));
	bwprintf("Size of double: %d\r\n", sizeof(double));
	bwprintf("Size of (void*): %d\r\n", sizeof(int*));
	bwprintf("Size of task descriptor: %d\n\r", sizeof(TaskDescriptor));
}
