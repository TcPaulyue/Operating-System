#include "semaphore.h"
Semaphore semaphore[10];

void W(Semaphore *s)
{
	if(s->next==NULL)
	{
		s->next=current;
		current->preIndex=-1;
		current->nextIndex=-1;
		current->state=BLOCKED;
		if(pcb_runnable==NULL)
			return;
		else
		{
			current=pcb_runnable;
			if(pcb_runnable->nextIndex==-1)
				pcb_runnable=NULL;
			else
			{
			pcb_runnable=&pcb[pcb_runnable->nextIndex];
			pcb_runnable->preIndex=-1;
			}
		}
		if(current==NULL)
			current=&idle;
	}
	else
	{
		PCB *temp=s->next;
		int index=temp->pid;
		while(pcb[index].nextIndex!=-1)
			index=pcb[index].nextIndex;
		current->preIndex=index;
		pcb[index].nextIndex=current->pid;
		current->state=BLOCKED;
		current->preIndex=-1;
		current->nextIndex=-1;
		if(pcb_runnable==NULL)
			return;
		else
		{
			current=pcb_runnable;
			if(pcb_runnable->nextIndex==-1)
				pcb_runnable=NULL;
			else
			{
				pcb_runnable=&pcb[pcb_runnable->nextIndex];
				pcb_runnable->preIndex=-1;
			}
		}
	}
}

void R(Semaphore *s)
{
	if(s->next==NULL)
		return;
	else
	{
		PCB *temp=s->next;
		int m=temp->pid;
		int k=pcb[m].nextIndex;
		pcb[k].preIndex=-1;
		s->next->pid=k;
		temp->state=RUNNABLE;
		temp->preIndex=-1;
		temp->nextIndex=-1;
		int index=pcb_runnable->pid;
		while(pcb[index].nextIndex!=-1)
			index=pcb[index].nextIndex;
		temp->preIndex=index;
		pcb[index].nextIndex=current->pid;
	}

}


void P(Semaphore *s)
{
    s->value--;
    if(s->value<0)
        W(s);
}
void V(Semaphore *s)
{
    s->value++;
    if(s->value<=0)
        R(s);
}
void seminit()
{
    int i=0;
    for(;i<10;i++)
    {
        semaphore[i].ifuse=0;
        semaphore[i].id=i;
        semaphore[i].value=0;
        semaphore[i].next=NULL;
    }
}
int getsem()
{
    int i=0;
    for(;i<10;i++)
    {
        if(semaphore[i].ifuse==0)
            break;
    }
    if(i!=10)
        return i;
    else
        return -1;
}
