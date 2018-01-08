#ifndef __lib_h__
#define __lib_h__
#include "types.h"
void printf(const char *format,...);
int fork();
void exit();
void sleep(unsigned);
int sem_init(sem_t *sem,uint32_t value);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_destroy(sem_t *sem);
#endif
