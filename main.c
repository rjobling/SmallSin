#include "support/gcc8_c_support.h"

#define TABLE_SIZE 1024

struct ExecBase* SysBase;

short JobboTable[TABLE_SIZE];
short DanTable[TABLE_SIZE];

int main()
{
	SysBase = *((struct ExecBase**) 4UL);

	register short* table ASM("a0");

	// Jobbo
	table = JobboTable;
	asm volatile(
	"		moveq	#0,%%d0				\n"
	".loop%=:							\n"
	"		move.l	%%d0,%%d1			\n"
	"		mulu	%%d1,%%d1			\n"
	"		swap	%%d1				\n"
	"		sub.l	%%d0,%%d1			\n"
	"		move.w	%%d1,(1024,%%a0)	\n"
	"		neg.w	%%d1				\n"
	"		move.w	%%d1,(%%a0)+		\n"
	"		add.w	#128,%%d0			\n"
	"		bne.s	.loop%=				\n"
	: [table] "+a" (table) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);

	// Dan
	table = DanTable;
	asm volatile(
	"		moveq	#0,%%d0			\n"
	"		move.w	#16384,%%d1		\n"
	"		moveq	#32,%%d2		\n"
	".loop%=:						\n"
	"		move.w	%%d0,%%d3		\n"
	"		muls	%%d1,%%d3		\n"
	"		asr.l	#8,%%d3			\n"
	"		asr.l	#4,%%d3			\n"
	"		move.w	%%d3,(%%a0)+	\n"
	"		neg.w	%%d3			\n"
	"		move.w	%%d3,1022(%%a0)	\n"
	"		add.w	%%d2,%%d0		\n"
	"		sub.w	%%d2,%%d1		\n"
	"		bgt.s	.loop%=			\n"
	: [table] "+a" (table) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);

	KPrintF("Jobbo\n");	for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", JobboTable[i]);
	KPrintF("Dan\n");	for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", DanTable[i]);
}
