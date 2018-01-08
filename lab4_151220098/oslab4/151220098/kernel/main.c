#include "common.h"
#include "x86.h"
#include "device.h"

void kEntry(void) {

	initSerial();// initialize serial port
	initIdt(); // initialize idt
	initIntr(); // iniialize 8259a
	initSeg(); // initialize gdt, tss
	initTimer();
	loadUMain(); // load user program, enter user space
	asm volatile("sti");
	while(1)
	{
		asm volatile("hlt");
		//putChar('f');
		//putChar('u');
		//putChar('c');
		//putChar('k');
	}
}
