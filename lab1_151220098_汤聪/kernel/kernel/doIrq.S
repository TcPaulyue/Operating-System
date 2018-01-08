.code32

.global irqEmpty
irqEmpty:
	pushl $0 // push dummy error code
	pushl $-1 // push interrupt vector into kernel
	jmp asmDoIrq

.global irqGProtectFault
irqGProtectFault:
	pushl $0xd
	jmp asmDoIrq

.global irqSyscall
irqSyscall:
	pushl $0 // push dummy error code
	pushl $0x80 // push interrupt vector into kernel stack
	jmp asmDoIrq

.global irqTimer
irqTimer:
	pushl $0
	pushl $0x20
	jmp asmDoIrq

.global asmDoIrq
asmDoIrq:
	cli
	pushal // push process state into kernel stack
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs

	pushl %esp
	movw $0x10,%ax
	movw %ax,%es
	movw %ax,%ds
	call irqHandle
	addl $4, %esp
	mov (current),%eax
	mov %eax, %esp
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popal
	addl $4, %esp //interrupt vector is on top of kernel stack
	addl $4, %esp //error code is on top of kernel stack
	sti
	iret
