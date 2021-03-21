#include "support/gcc8_c_support.h"

#define TABLE_SIZE 1024

struct ExecBase* SysBase;

short JobboSize;
short JobboTable[TABLE_SIZE];

short DanSize;
short DanTable[TABLE_SIZE];

short Dan2Size;
short Dan2Table[TABLE_SIZE];

short ABSize;
short ABTable[TABLE_SIZE];

short AB2Size;
short AB2Table[TABLE_SIZE];

short AB3Size;
short AB3Table[TABLE_SIZE];

short RossSize;
short RossTable[TABLE_SIZE];

int main()
{
	SysBase = *((struct ExecBase**) 4UL);

	register short* table ASM("a0");
	register short size ASM("d7");

	// Jobbo
	table = JobboTable;
	asm volatile(
	"start%=:							\n"
	"		moveq	#0,%%d0				\n"
	"loop%=:							\n"
	"		move.w	%%d0,%%d1			\n"
	"		mulu	%%d1,%%d1			\n"
	"		swap	%%d1				\n"
	"		sub.w	%%d0,%%d1			\n"
	"		move.w	%%d1,(1024,%%a0)	\n"
	"		neg.w	%%d1				\n"
	"		move.w	%%d1,(%%a0)+		\n"
	"		add.w	#128,%%d0			\n"
	"		bne.s	loop%=				\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	JobboSize = size;

	// Dan
	table = DanTable;
	asm volatile(
	"start%=:							\n"
	"		moveq	#0,%%d0				\n"
	"		move.w	#16384,%%d1			\n"
	"		moveq	#32,%%d2			\n"
	"loop%=:							\n"
	"		move.w	%%d0,%%d3			\n"
	"		muls	%%d1,%%d3			\n"
	"		asr.l	#8,%%d3				\n"
	"		asr.l	#4,%%d3				\n"
	"		move.w	%%d3,(%%a0)+		\n"
	"		neg.w	%%d3				\n"
	"		move.w	%%d3,1022(%%a0)		\n"
	"		add.w	%%d2,%%d0			\n"
	"		sub.w	%%d2,%%d1			\n"
	"		bgt.s	loop%=				\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	DanSize = size;

	/* Overflows the table and causes problems.
	// Dan 2
	table = Dan2Table;
	asm volatile(
			// Entry a0 = Beginning of 2048 byte sine table buffer
	"start%=:							\n"
	"		lea		512(%%a0),%%a0		\n"	// a0 = 2nd Quadrant Start
	"		lea		2(%%a0),%%a1		\n"	// a1 = 1st Quadrant End + 2
	"		moveq	#11,%%d0			\n"	// d0.l = x = 11
	"		moveq	#1,%%d1				\n"
	"		ror.w	#2,%%d1				\n"	// d1.l = y = 16384
	"		move.w	#163,%%d2			\n"	// d2 = Q = magic division value = 512/PI (162.97466)
	"loop%=:							\n"
	"		move.l	%%d1,%%d3			\n"
	"		divu	%%d2,%%d3			\n"
	"		add.w	%%d3,%%d0			\n"	// x = x + (y / Q)
	"		move.l	%%d0,%%d3			\n"
	"		divu	%%d2,%%d3			\n"
	"		sub.w	%%d3,%%d1			\n"	// y = y - (x / Q)
	"		move.w	%%d1,%%d3			\n"
	"		neg.w	%%d3				\n"
	"		move.w	%%d3,1024(%%a0)		\n"	// write 4th Quadrant
	"		move.w	%%d3,1022(%%a1)		\n"	// write 3rd Quadrant
	"		move.w	%%d1,(%%a0)+		\n"	// Write 2nd Quadrant
	"		move.w	%%d1,-(%%a1)		\n"	// Write 1st Quadrant
	"		addq.w	#1,%%d3				\n"
	"		blt.s	loop%=				\n"
	"		clr.w	(%%a1)				\n"	// Set SinTable index 0 to 0
	"		clr.w	-(%%a0)				\n"	// Set SinTable index 512 to 0
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	Dan2Size = size;
	*/

	// A/B
	table = ABTable;
	asm volatile(
	"start%=:							\n"
	"		moveq	#0,%%d0				\n"
	"		move.w	#512,%%d1			\n"
	"loop%=:							\n"
	"		move.w	%%d0,%%d2			\n"
	"		muls.w	%%d1,%%d2			\n"
	"		asr.l	#2,%%d2				\n"
	"		move.w	%%d2,(%%a0)+		\n"
	"		neg.w	%%d2				\n"
	"		move.w	%%d2,(1024-2,%%a0)	\n"
	"		addq.w	#1,%%d0				\n"
	"		subq.w	#1,%%d1				\n"
	"		bgt.b	loop%=				\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	ABSize = size;

	// A/B 2
	table = AB2Table;
	asm volatile(
	"start%=:							\n"
	"		moveq	#0,%%d0				\n"
	"		lea	(511+2).w,%%a1			\n"
	"loop%=:							\n"
	"		subq.l	#2,%%a1				\n"
	"		move.l	%%d0,%%d2			\n"
	"		asr.l	#2,%%d2				\n"
	"		move.w	%%d2,(%%a0)+		\n"
	"		neg.w	%%d2				\n"
	"		move.w	%%d2,(1024-2,%%a0)	\n"
	"		add.l	%%a1,%%d0			\n"
	"		bne.b	loop%=				\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	AB2Size = size;

	// A/B 3
	table = AB3Table;
	asm volatile(
	"start%=:							\n"
	"		moveq	#0,%%d0				\n"
	"		move.w	#511+2,%%a1			\n"
	"loop%=:							\n"
	"		subq.l	#2,%%a1				\n"
	"		move.l	%%d0,%%d1			\n"
	// extra accuracy begin
	"		move.w	%%d1,%%d2			\n"
	"		not.w	%%d2				\n"
	"		mulu.w	%%d1,%%d2			\n"
	"		divu.w	#75781/2,%%d2		\n"
	"		lsr.w	#3,%%d2				\n"
	"		sub.w	%%d2,%%d1			\n"
	// extra accuracy end
	"		asr.l	#2,%%d1				\n"
	"		move.w	%%d1,(%%a0)+		\n"
	"		neg.w	%%d1				\n"
	"		move.w	%%d1,(1024-2,%%a0)	\n"
	"		add.l	%%a1,%%d0			\n"
	"		bne.b	loop%=				\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	AB3Size = size;

	// Ross
	table = RossTable;
	asm volatile(
	"start%=:							\n"
	"		lea		1024*2(%%a0),%%a1	\n"
	"		moveq	#97,%%d0			\n"
	"		ror.l	#7,%%d0				\n"
	"		moveq	#0,%%d1				\n"
	".l%=:	move.l	%%d0,%%d3			\n"
	"		move.l	%%d1,%%d2			\n"
	"		mulu.w	%%d2,%%d2			\n"
	"		sub.l	%%d2,%%d3			\n"
	"		swap	%%d3				\n"
	"		mulu.w	%%d1,%%d3			\n"
	"		swap	%%d3				\n"
	"		move.w	%%d3,(%%a0)+		\n"
	"		move.w	%%d3,-2-1024(%%a1)	\n"
	"		neg.w	%%d3				\n"
	"		beq.b	.2%=				\n"
	"		move.w	%%d3,-(%%a1)		\n"
	".2%=:	move.w	%%d3,-2+1024(%%a0)	\n"
	"		addi.w	#1<<7,%%d1			\n"
	"		bpl.b	.l%=				\n"
	"		bvs.b	.l%=				\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	RossSize = size;

	KPrintF("Jobbo (%ld bytes)\n", JobboSize); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", JobboTable[i]);
	KPrintF("Dan (%ld bytes)\n"  , DanSize  ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", DanTable[i]  );
	//KPrintF("Dan 2 (%ld bytes)\n", Dan2Size ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", Dan2Table[i] );
	KPrintF("A/B (%ld bytes)\n"  , ABSize   ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", ABTable[i]   );
	KPrintF("A/B 2 (%ld bytes)\n", AB2Size  ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", AB2Table[i]  );
	KPrintF("A/B 3 (%ld bytes)\n", AB3Size  ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", AB3Table[i]  );
	KPrintF("Ross (%ld bytes)\n" , RossSize ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", RossTable[i] );
}
