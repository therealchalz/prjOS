#include <uart.h>
#include <bwio.h>
#include <cpu.h>
#include <board.h>


#ifdef DEBUG
void __error__(char *filename, unsigned long line) {
	//TODO: something useful
}
#endif


int main(void) {

	cpuInit();

	boardInit();

	UARTInit(115200);

	bwprintf("\n\n\n********Kernel Starting********\n\r\n");

	int testLoop = 10;

	while(testLoop > 0)
	{
		testLoop--;

		boardSetIndicatorLED(1);
		long l = 0x000fffff;
		while (l > 0) {
			l--;
		}
		boardSetIndicatorLED(0);
		l = 0x000fffff;
		while (l > 0) {
			l--;
		}
		bwprintf("Running!\r\n");
	}

	return 420;
}

