#include "support/gcc8_c_support.h"

#define TABLE_SIZE 1024

struct ExecBase* SysBase;

int JobboSize;
short JobboTable[TABLE_SIZE];

int DanSize;
short DanTable[TABLE_SIZE];

int ABSize;
short ABTable[TABLE_SIZE];

int AB2Size;
short AB2Table[TABLE_SIZE];

int main()
{
	SysBase = *((struct ExecBase**) 4UL);

	register short* table ASM("a0");
	register int size ASM("d7");

	// Jobbo
	table = JobboTable;
	asm volatile(
	".start%=:								\n"
	"		moveq	#0,%%d0					\n"
	".loop%=:								\n"
	"		move.w	%%d0,%%d1				\n"
	"		mulu	%%d1,%%d1				\n"
	"		swap	%%d1					\n"
	"		sub.w	%%d0,%%d1				\n"
	"		move.w	%%d1,(1024,%%a0)		\n"
	"		neg.w	%%d1					\n"
	"		move.w	%%d1,(%%a0)+			\n"
	"		add.w	#128,%%d0				\n"
	"		bne.s	.loop%=					\n"
	".end%=:								\n"
	"		move.l	#.end%=-.start%=,%%d7	\n"
	: "+a" (table) : "d" (size) : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	JobboSize = size;

	// Dan
	table = DanTable;
	asm volatile(
	".start%=:								\n"
	"		moveq	#0,%%d0					\n"
	"		move.w	#16384,%%d1				\n"
	"		moveq	#32,%%d2				\n"
	".loop%=:								\n"
	"		move.w	%%d0,%%d3				\n"
	"		muls	%%d1,%%d3				\n"
	"		asr.l	#8,%%d3					\n"
	"		asr.l	#4,%%d3					\n"
	"		move.w	%%d3,(%%a0)+			\n"
	"		neg.w	%%d3					\n"
	"		move.w	%%d3,1022(%%a0)			\n"
	"		add.w	%%d2,%%d0				\n"
	"		sub.w	%%d2,%%d1				\n"
	"		bgt.s	.loop%=					\n"
	".end%=:								\n"
	"		move.l	#.end%=-.start%=,%%d7	\n"
	: "+a" (table) : "d" (size) : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	DanSize = size;

	// A/B
	table = ABTable;
	asm volatile(
	".start%=:								\n"
	"		moveq	#0,%%d0					\n"
	"		move.w	#512,%%d1				\n"
	"Loop3%=:								\n"
	"		move.w	%%d0,%%d2				\n"
	"		muls.w	%%d1,%%d2				\n"
	"		asr.l	#2,%%d2					\n"
	"		move.w	%%d2,(%%a0)+			\n"
	"		neg.w	%%d2					\n"
	"		move.w	%%d2,(1024-2,%%a0)		\n"
	"		addq.w	#1,%%d0					\n"
	"		subq.w	#1,%%d1					\n"
	"		bgt.b	Loop3%=					\n"
	".end%=:								\n"
	"		move.l	#.end%=-.start%=,%%d7	\n"
	: "+a" (table) : "d" (size) : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	ABSize = size;

	// A/B 2
	table = AB2Table;
	asm volatile(
	".start%=:								\n"
	"		moveq	#0,%%d0					\n"
	"		lea	(511+2).w,%%a1				\n"
	"Loop4%=:								\n"
	"		subq.l	#2,%%a1					\n"
	"		move.l	%%d0,%%d2				\n"
	"		asr.l	#2,%%d2					\n"
	"		move.w	%%d2,(%%a0)+			\n"
	"		neg.w	%%d2					\n"
	"		move.w	%%d2,(1024-2,%%a0)		\n"
	"		add.l	%%a1,%%d0				\n"
	"		bne.b	Loop4%=					\n"
	".end%=:								\n"
	"		move.l	#.end%=-.start%=,%%d7	\n"
	: "+a" (table) : "d" (size) : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	AB2Size = size;

	KPrintF("Jobbo (%ld bytes)\n", JobboSize); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", JobboTable[i]);
	KPrintF("Dan (%ld bytes)\n"  , DanSize  ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", DanTable[i]  );
	KPrintF("A/B (%ld bytes)\n"  , ABSize   ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", ABTable[i]   );
	KPrintF("A/B 2(%ld bytes)\n" , AB2Size  ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", AB2Table[i]  );
}
