#include "boot.h"

#define SECTSIZE 512
static void readseg(unsigned char*,unsigned int,unsigned int);
void bootMain(void) {
	/* 加载内核至内存，并跳转执行 */
	struct ELFHeader *elf;//elf文件头指针
	struct ProgramHeader *ph,*eph;//一个是下界（开始位置），一个是上界(结束位置)
	elf=(struct ELFHeader*)0x8000;
	readseg((unsigned char *)elf,0,4096);//一次读取8个扇区，为嘛？pa上就是这样写的。。。。
	ph=(struct ProgramHeader*)((char *)elf+elf->phoff);//程序头表的入口。
	eph = ph + elf->phnum;//需要加载的程序头表上界
	for(;ph<eph;ph++)
	{
		//该段加载到物理内存的位置
		readseg((unsigned char *)(ph->paddr),ph->off,ph->filesz);
	}
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
static void readseg(unsigned char *pa,unsigned int offset,unsigned int len)
{
	//offset指的是相对elf文件头的偏移位置
	//pa指的是代码加载到的内存位置
	//len指的是加载的代码长度
	unsigned char *epa;//上界
	epa = pa + len;//计算上界
	pa -= offset % SECTSIZE;//在这里改变pa不会影响实际参数，计算的是相对于一个扇区的偏移量
	offset = (offset/SECTSIZE)+1;//因为调用的是readSect函数，该函数一次读取一个扇区，本次实验中，没有别的驱动器号，磁头号，柱头号，统统为0，所以offset就代表了扇区号。注：0号扇区是bootloader，内核elf是从第1号扇区开始的。
	for(;pa<epa;pa+=SECTSIZE,offset++)
	{
		readSect(pa,offset);
	}
}
