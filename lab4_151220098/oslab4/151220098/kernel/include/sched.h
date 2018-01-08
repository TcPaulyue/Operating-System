#include "x86.h"
#define MAX_STACK_SIZE 4096
#define RUNNABLE 1
#define RUNNING 2
#define BLOCKED 3
#define DEAD 0
#define MAX_PCB_NUM 100
TSS tss;
extern int page[MAX_PCB_NUM];
typedef struct ProcessTable{
	struct TrapFrame tf;
	uint32_t stack[MAX_STACK_SIZE];
	int state;
	int timeCount;
	int sleepTime;
	uint32_t pid;
	uint32_t father;
	int nextIndex;
	int preIndex;
}PCB;

extern PCB pcb[MAX_PCB_NUM];
extern PCB *pcb_runnable;
extern PCB *pcb_blocked;
extern PCB *current;
extern PCB idle;
void init_pcb();
void blocked2runnable();
void schedule();
void FORK();
void EXIT();
void SLEEP(unsigned);
void putint(uint32_t s);