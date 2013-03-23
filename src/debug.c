/*
 * debug.c
 */

#include <debug.h>
#include <bwio.h>

// byteGroupSize={1,2,4}
// showAsLittleEndian={0,!0}

void prettyPrintMemory(char *location, int numBytes, char byteGroupSize, char showAsLittleEndian) {
	if (numBytes == 0)
		return;
	//Word aligned print
	int bytesToSkip = ((int)location % 16);
	location -= bytesToSkip;

	numBytes += bytesToSkip;
	int bytesLeft = numBytes;

	while (bytesLeft > 0) {
		bwprintf("%08x", location);
		int bytesThisLine = 16;
		if (bytesLeft < bytesThisLine)
			bytesThisLine = bytesLeft;
		char groupSize = 0;
		int b;
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
void prettyPrintMemoryBytes(char *location, int numBytes) {
	prettyPrintMemory(location, numBytes, 1, 0);

}
void prettyPrintMemory32Bit(char* location, int numWords) {
	prettyPrintMemory(location, numWords*4, 4, 1);
}
void printStackTop(char* stackLocation, int numItems) {
	prettyPrintMemory32Bit(stackLocation, numItems);
}
void printCurrentStackTop(int numItems) {
	int x = 4;
	int *px = &x;
	int skipAmount = 5;
	printStackTop((char*)(px+skipAmount), numItems);
}
