//#include "x86.h"
//#include "device.h"
//#include "sched.h"
#include "semaphore.h"
void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);
/*void putint(uint32_t s)
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
}*/
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
			//putChar('h');
			if(current!=&idle)
				current->timeCount--;
			blocked2runnable();
			//putChar('b');
			break;
		default:assert(0);
	}
	schedule();	
}

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
		//putChar('f');
		}
	else if(tf->eax==4)
		{
		SLEEP(tf->ecx);
		//putChar('s');		
		}
	else if(tf->eax==5)
		{
		EXIT();
		//putChar('e');		
		}
	else if(tf->eax==6)
		{
			int id=getsem();
			if(id==-1)
			{	tf->eax=-1;
				return;
			}
			semaphore[id].ifuse=1;
			semaphore[id].value=tf->ecx;
			semaphore[id].next=NULL;
			tf->eax=id;	
		}
	else if(tf->eax==7)
		{
			/*if(tf->ecx>10||tf->ecx<0||semaphore[tf->ecx].ifuse==0)
			{
				tf->eax=-1;
				return;
			}*/
			V(&semaphore[tf->ecx]);
			tf->eax=semaphore[tf->ecx].value;
		}
	else if(tf->eax==8)
		{
			/*if(tf->ecx>10||tf->ecx<0||semaphore[tf->ecx].ifuse==0)
			{
				tf->eax=-1;
				return;
			}*/
			P(&semaphore[tf->ecx]);
			tf->eax=semaphore[tf->ecx].value;
		}
	else if(tf->eax==9)
		{
			/*if(tf->ecx>10||tf->ecx<0||semaphore[tf->ecx].ifuse==0)
			{
				tf->eax=-1;
				return;
			}*/
			semaphore[tf->ecx].ifuse=0;
			semaphore[tf->ecx].value=0;
			semaphore[tf->ecx].next=NULL;
		}
	asm volatile("pop %edi");
}
void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}
