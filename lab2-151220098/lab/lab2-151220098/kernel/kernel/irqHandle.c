#include "x86.h"
#include "device.h"

void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */

	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x80:
			syscallHandle(tf);
			break;
		default:assert(0);
	}
}
int location=0;
void syscallHandle(struct TrapFrame *tf) {
	//printf();
	if(tf->eax==2)
	{
		if(tf->ecx=='\n')
		{
			location=location+(160-location%160);
			return;
		}
		asm volatile("movw %%ax,%%gs"::"a"(KSEL(SEG_VEDIO)));
		asm volatile("movb $0x0c,%ah");
		asm volatile("movb %0,%%al"::"m"(tf->ecx));
		asm volatile("movl %0,%%edi"::"m"(location));
		asm volatile("movw %ax,%gs:(%edi)");
		location+=2;	
	}
}
void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}
