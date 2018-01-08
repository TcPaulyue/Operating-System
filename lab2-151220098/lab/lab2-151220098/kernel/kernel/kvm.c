#include "x86.h"
#include "device.h"
SegDesc gdt[NR_SEGMENTS];
TSS tss;

#define SECTSIZE 512

void waitDisk(void) {
	while((inByte(0x1F7) & 0xC0) != 0x40); 
}

void readSect(void *dst, int offset) {
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
	offset = offset / SECTSIZE + 201;
	for(;pa < epa; offset++, pa = pa + SECTSIZE)
	{
		readSect((void *)pa, offset);
	}
}
void initSeg() {
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
	gdt[SEG_VEDIO] = SEG(STA_W,	    0xb8000, 0xffffffff, DPL_KERN);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
	gdt[SEG_TSS].s = 0;
	setGdt(gdt, sizeof(gdt));

	/*
	 * 初始化TSS
	 */
	asm volatile("movl %%esp,%0":"=m"(tss.esp0));
	tss.ss0=KSEL(SEG_KDATA);
	asm volatile("ltr %%ax":: "a" (KSEL(SEG_TSS)));

	/*设置正确的段寄存器*/
	asm volatile("movw %%ax,%%es"::"a"(KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds"::"a"(KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss"::"a"(KSEL(SEG_KDATA)));

	lLdt(0);
	
}

void enterUserSpace(uint32_t entry) {
	/*
	 * Before enter user space 
	 * you should set the right segment registers here
	 * and use 'iret' to jump to ring3
	 */

	asm volatile("movw %%ax,%%es"::"a"(USEL(SEG_UDATA)));
	asm volatile("movw %%ax,%%ds"::"a"(USEL(SEG_UDATA)));
	asm volatile("pushl %0"::"a"(USEL(SEG_UDATA)));
	asm volatile("pushl $0x300000");
	asm volatile("pushl $0x202");
	asm volatile("pushl %0"::"a"(USEL(SEG_UCODE)));
	asm volatile("pushl %0"::"a"(entry));
	asm volatile("iret");
}
static unsigned char buf[4096];
void loadUMain(void) {

	/*加载用户程序至内存*/
	struct ELFHeader *elf;
	struct ProgramHeader *ph,*eph;
	elf=(struct ELFHeader*)buf;
	//readseg((unsigned char*)elf,0,4096);
	int i=0;
	for(;i<8;i++)
	{
		readSect(buf+i*512,201+i);
	}
	ph=(struct ProgramHeader*)((char *)elf+elf->phoff);
	eph=ph+elf->phnum;
	for(;ph<eph;ph++)
	{
		readseg((unsigned char *)(ph->paddr),ph->off,ph->filesz);
	}
	enterUserSpace(elf->entry);
}
