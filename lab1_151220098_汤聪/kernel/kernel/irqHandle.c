#include "x86.h"
#include "device.h"
#include "sched.h"
void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);
void putint(uint32_t s)
{
	if(s==-1)
	{
		putChar('-');
		putChar('1');
		return;
	}
	if(s/10!=0)
		putint(s/10);
	putChar(s%10+'0');
}
void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */
	current->tf=*tf;
	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x80:
			syscallHandle(tf);
			break;
		case 0x20:
			putChar('h');
			if(current!=&idle)
				current->timeCount--;
			blocked2runnable();
			putChar('b');
			break;
		default:assert(0);
	}
	schedule();	
}
/*void roll()
{
	asm volatile("movw %%ax,%%gs"::"a"(KSEL(SEG_VEDIO)));
	int i=0;
	asm volatile("push %edi");
	for(;i<25*80*2;i=i+2)
	{
		int j= i + 160;
		asm volatile("movl %0,%%edi"::"m"(j));
		asm volatile("movw %gs:(%edi),%bx");
		asm volatile("movl %0,%%edi"::"m"(i));
		asm volatile("movw %bx,%gs:(%edi)");
	}
	asm volatile("pop %edi");
}*/
int location=0;
void syscallHandle(struct TrapFrame *tf) {
	asm volatile("push %edi");
	/* 实现系统调用*/
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
	else if(tf->eax==3)
		{
		FORK();
		putChar('f');
		}
	else if(tf->eax==4)
		{
		SLEEP(tf->ecx);
		putChar('s');		
		}
	else if(tf->eax==5)
		{
		EXIT();
		putChar('e');		
		}
	asm volatile("pop %edi");
}
void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}
