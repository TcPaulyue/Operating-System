#include "lib.h"
#include "types.h"
/*
 * io lib here
 * 库函数写在这
 */
int32_t syscall(int num, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret = 0;

	/* 内嵌汇编 保存 num, a1, a2, a3, a4, a5 至通用寄存器*/
	asm volatile("int $0x80" : "=a"(ret): "a"(num), "b"(a1), "c"(a2), "d"(a3));
	return ret;
}
void printch(char ch)
{
	syscall(2,0,ch,0,0,0);
}
void printstr(char* str)
{
	int i=0;
	for(;str[i]!='\0';i++)
	{
		syscall(2,0,str[i],0,0,0);
	}
}
void printdec(int dec)
{
	if(dec==0)
	{
		syscall(2,0,'0',0,0,0);
		return;
	}
	if(dec<0)
	{
		syscall(2,0,'-',0,0,0);
		dec=-dec;
	}
	int temp=dec;
	char t[20];
	int count=0;
	for(;temp>0;temp=temp/10)
	{
		t[count]=(char)(temp%10)+'0';
		count++;
	}
	int i=0;
	for(;i<=count-1;i++)
		syscall(2,0,t[count-1-i],0,0,0);
}
void printhex(unsigned hex)
{
	if(hex==0)
	{
		syscall(2,0,'0',0,0,0);		
		return;
	}
	char t[20];
	int count=0;
	unsigned temp=hex;
	for(;temp>0;temp=temp/16)
	{
		if(temp%16<10)
			t[count]=(char)(temp%16)+'0';
		else
			t[count]=(char)((temp%16)-10)+'a';
		count++;
	}
	int i=0;
	for(;i<=count-1;i++)
		syscall(2,0,t[count-1-i],0,0,0);
}

void printf(const char *format,...){
	char* pArg = 0;
	pArg = (char*)(&format);
	pArg += sizeof(format);
	while(*format)
	{
		if((*format) == '%')
		{
			switch(*(++format))
			{
				case 'c':printch(*((char*)pArg));break;
				case 's':printstr((*(char**)pArg));break;
				case 'd':printdec(*((int*)pArg));break;
				case 'x':printhex(*((unsigned*)pArg));break;
			}
			pArg += sizeof(int);
		}
		else
		{
			syscall(2,0,*format,0,0,0);
		}
		++format;
	}
}
int fork()
{
	return syscall(3,0,0,0,0,0);
	
}
void sleep(unsigned time)
{
	syscall(4,0,time,0,0,0);
}
void exit()
{
	syscall(5,0,0,0,0,0);
}

int sem_init(sem_t *sem,uint32_t value)
{
	sem->value = value;
	sem->id = syscall(6,0,value,0,0,0);
	if(sem->id==-1)
		return -1;
	else 
		return 0;
}
int sem_post(sem_t *sem)
{
	int ret=syscall(7,0,sem->id,0,0,0);
	if(ret==-1)
		return -1;
	else 
	{
		sem->value=ret;
		return 0; 
	}
}
int sem_wait(sem_t *sem)
{
	int ret=syscall(8,0,sem->id,0,0,0);
	if(ret==-1)
		return -1;
	else 
	{
		sem->value=ret;
		return 0; 
	}
}
int sem_destroy(sem_t *sem)
{
	int ret=syscall(9,0,sem->id,0,0,0);
	if(ret==-1)
		return -1;
	else 
		return 0;
}
