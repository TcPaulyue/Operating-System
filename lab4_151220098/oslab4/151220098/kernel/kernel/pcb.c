#include "sched.h"
#include "x86.h"
#include "device.h"
#define HZ 10
extern TSS tss;
extern SegDesc gdt[NR_SEGMENTS];
PCB pcb[MAX_PCB_NUM];
PCB *pcb_runnable = NULL;
PCB *pcb_blocked = NULL;
PCB *current = NULL;
PCB idle;
void init_pcb(void)
{
	int i=0;
	for(i=0;i<MAX_PCB_NUM;i++)
	{
		pcb[i].state = DEAD;
		pcb[i].timeCount = 0;
		pcb[i].sleepTime = 0;
		pcb[i].pid = i;
		pcb[i].father = -1;
		pcb[i].preIndex = -1;
		pcb[i].nextIndex = -1;
	}
	idle.timeCount = 10;
	idle.pid = 9;
}
void blocked2runnable()
{
	if(pcb_blocked==NULL)
		return;
	int index = pcb_blocked->pid;
	while(index!=-1)
	{
		int temp = pcb[index].nextIndex;
		pcb[index].sleepTime--;
		if(pcb[index].sleepTime<=0)
		{
			pcb[index].state=RUNNABLE;
			if(pcb[index].preIndex==-1&&pcb[index].nextIndex==-1)//block队列中只有这一个进程
				pcb_blocked = NULL;
			else if(pcb[index].preIndex==-1)
			{
				pcb_blocked = &pcb[pcb[index].nextIndex];
				pcb_blocked->preIndex=-1;
			}
			else
			{
				pcb[pcb[index].preIndex].nextIndex = pcb[index].nextIndex;
				if(pcb[index].nextIndex!=-1)
					pcb[pcb[index].nextIndex].preIndex = pcb[index].preIndex;
			}
			pcb[index].nextIndex = -1;
			pcb[index].preIndex = -1;
			if(pcb_runnable==NULL)
				pcb_runnable=&pcb[index];
			else
			{
				int rindex = pcb_runnable->pid;
				while(pcb[rindex].nextIndex != -1)
					rindex = pcb[rindex].nextIndex;
				pcb[rindex].nextIndex=index;
				pcb[index].preIndex = rindex;
				pcb[index].nextIndex = -1;
			}
		}
		index=temp;
	}
}
void schedule()
{
	if(current == &idle)
	{
		
		if(pcb_runnable!=NULL)
		{
			current = pcb_runnable;
			current->state=RUNNING;
			current->timeCount = 10;
			tss.esp0 = (int) current->stack + 4096;
			if(pcb_runnable->nextIndex!=-1)
			{
				pcb_runnable = &pcb[pcb_runnable->nextIndex];
				pcb_runnable->preIndex = -1;
			}
			else
			{
				pcb_runnable = NULL;
			}
			current->preIndex = -1;
			current->nextIndex =-1;
			current->state = RUNNING;
		}
		return;
	}
	if(current->timeCount == 0)
	{
		current->state = RUNNABLE;
		if(pcb_runnable==NULL)
		{
			current->timeCount = 10;
		}
		else
		{
			int index = pcb_runnable->pid;
			while(pcb[index].nextIndex != -1)
				index = pcb[index].nextIndex;
			pcb[index].nextIndex = current->pid;
			current->preIndex = index;
			current = pcb_runnable;
			pcb_runnable = &pcb[pcb_runnable->nextIndex];
			pcb_runnable->preIndex=-1;
			current->nextIndex=-1;
			current->state = RUNNING;
		}
	}
}
void SLEEP(unsigned time)
{
	
	current->sleepTime = time;
	current->state = BLOCKED;
	current->preIndex=-1;
	current->nextIndex=-1;
	if(pcb_blocked==NULL)
	{
		pcb_blocked=current;
	}
	else
	{
		int index=pcb_blocked->pid;
		while(pcb[index].nextIndex!=-1)
			index=pcb[index].nextIndex;
		current->preIndex=index;
		pcb[index].nextIndex=current->pid;
	}
	current=&idle;
	if(pcb_runnable==NULL)
		return;
	else
	{
		current = pcb_runnable;
		current->timeCount = 10;
		if(pcb_runnable->nextIndex==-1)
		{
			pcb_runnable = NULL;
		}
		else
		{
			pcb_runnable = &pcb[pcb_runnable->nextIndex];
			pcb_runnable->preIndex = -1;
		}
		current->nextIndex = -1;
		current->state = RUNNING;
		tss.esp0 = (int)current->stack + 4096;
	}
}
int page[MAX_PCB_NUM]={0};
void FORK()
{
	int i=0;
	for(;i<MAX_PCB_NUM;i++)
	{
		if(pcb[i].state == DEAD)
			break;
	}
	pcb[i]=*current;
	pcb[i].pid=i;
	int pageindex = 0;
	for(;pageindex<MAX_PCB_NUM;pageindex++)
	{
		if(page[pageindex]==0)
			break;
	}
	page[pageindex]=1;
	int newesp = current->tf.esp - ((current->tf.esp>>20)<<20)+(pageindex<<20);
	int newebp = current->tf.ebp - ((current->tf.ebp>>20)<<20)+(pageindex<<20);
	pcb[i].tf = current->tf;
	pcb[i].tf.eax = 0;
	pcb[i].tf.esp = newesp;
	pcb[i].tf.ebp = newebp;
	int p = current->tf.esp;
	int old_upper = ((current->tf.esp>>20)<<20)+0x100000;
	for(;p < old_upper;p+=sizeof(char))
	{
		*(unsigned char*)(newesp) = *(unsigned char *)(p);
		newesp+=sizeof(char);
	}
	current->tf.eax = i;
	pcb[i].state = RUNNABLE;
	pcb[i].timeCount = 10;
	pcb[i].sleepTime = 0;
	pcb[i].father = current->pid;
	pcb[i].nextIndex = -1;
	pcb[i].preIndex = -1;
	if(pcb_runnable == NULL)
	{
		pcb_runnable = &pcb[i];
	}
	else
	{
		int index = pcb_runnable->pid;
		while(pcb[index].nextIndex!=-1)
		{
			index=pcb[index].nextIndex;
		}
		pcb[i].preIndex = index;
		pcb[index].nextIndex = i;
	}
	/*gdt[7]=SEG(STA_X|STA_R,0x100000,0x100000,DPL_USER);
	gdt[8]=SEG(STA_W,0x100000,0x100000,DPL_USER);
	int i;
	for(i=0;i<0x100000;i++)
		*(char *)(0x300000+i)=*(char *)(0x200000+i);
	for(i=0;i<MAX_PCB_NUM;i++)
	{
		if(pcb[i].state==DEAD)
			break;
	}
	pcb[i]=*current;
	pcb[i].pid=i;
//	pcb[i].tf=(void*)(((uint32_t)pcb[i]-(uint32_t)current)+(uint32_t)current->tf);
	pcb[i].tf=current->tf;
	pcb[i].tf.ds=(USEL(8));
	pcb[i].tf.es=(USEL(8));
	pcb[i].tf.ss=(USEL(8));
	pcb[i].tf.cs=(USEL(7));
	pcb[i].state=RUNNABLE;
	pcb[i].timeCount=10;
	pcb[i].tf.eax=0;
	current->tf.eax=0;
	if(pcb_runnable==NULL)
		pcb_runnable=&pcb[i];
	else
	{
		int index=pcb_runnable->pid;
		while(pcb[index].nextIndex!=-1)
			index=pcb[index].nextIndex;
		pcb[i].preIndex=index;
		pcb[index].nextIndex=i;
	}*/
}
void EXIT()
{
	current->state = DEAD;
	if(pcb_runnable == NULL)
	{
		current = &idle;
	}
	else
	{
		current = pcb_runnable;
		if(pcb_runnable->nextIndex == -1)
		{
			pcb_runnable = NULL;
		}
		else
		{
			pcb_runnable = &pcb[pcb_runnable->nextIndex];
			pcb_runnable->preIndex = -1;
		}
		tss.esp0 = (int)current->stack + 4096;
	}
}












