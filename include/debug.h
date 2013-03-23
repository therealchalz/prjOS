/*
 * debug.h
 */

#ifndef DEBUG_H_
#define DEBUG_H_

void printCurrentStackTop(int numItems);
void printStackTop(char* stackLocation, int numItems);
void prettyPrintMemory(char *location, int numBytes, char byteGroupSize, char showAsLittleEndian);

#endif /* DEBUG_H_ */
