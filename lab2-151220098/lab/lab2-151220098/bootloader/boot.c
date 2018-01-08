#include "boot.h"

#define SECTSIZE 512
static void readseg(unsigned char*,unsigned int,unsigned int);
void bootMain(void) {
	/* 加载内核至内存，并跳转执行 */
	struct ELFHeader *elf;
	struct ProgramHeader *ph,*eph;
	elf=(struct ELFHeader*)0x8000;
	readseg((unsigned char *)elf,0,4096);
	ph=(struct ProgramHeader*)((char *)elf+elf->phoff);
	eph = ph + elf->phnum;
	for(;ph<eph;ph++)
		readseg((unsigned char *)(ph->paddr),ph->off,ph->filesz);
	((void (*)(void))(elf->entry))();
	while(1);
}

static void waitDisk(void) { // waiting for disk
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

static void readSect(void *dst, int offset) { // reading a sector of disk
	int i;
	waitDisk();
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}
void readseg(unsigned char *pa,unsigned int offset,unsigned int len)
{
	unsigned char *epa;
	epa = pa + len;
	pa -= offset % SECTSIZE;
	offset = (offset/SECTSIZE)+1;
	for(;pa<epa;pa+=SECTSIZE,offset++)
		readSect(pa,offset);
}
