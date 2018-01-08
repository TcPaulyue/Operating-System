#include "x86.h"
#include "sched.h"
typedef struct Semaphore
{
	int value;
	int id;
	int ifuse;
	PCB *next;
}Semaphore;
Semaphore semaphore[10];
void P(Semaphore *s);
void V(Semaphore *s);
void seminit();
int getsem();
