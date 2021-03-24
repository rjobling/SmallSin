#include "support/gcc8_c_support.h"

#define TABLE_SIZE 1024

struct ExecBase* SysBase;

short JobboSize;
short JobboTable[TABLE_SIZE];

short Jobbo2Size;
short Jobbo2Table[TABLE_SIZE];

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

	// Jobbo 2
	table = Jobbo2Table;
	asm volatile(
	"start%=:							\n"
	"		moveq	#0,%%d0				\n"
	"		move.w	#512,%%d1			\n"
	".loop%=:							\n"
	"		move.w	%%d1,%%d2			\n"
	"		mulu	%%d0,%%d2			\n"	// d2 = (512-x)*x

	"		move.l	%%d2,%%d3			\n"
	"		lsl.l	#8,%%d3				\n"
	"		lsl.l	#5,%%d3				\n" // d3 = 8192*(512-x)*x

	"		lsr.l	#3,%%d2				\n"
	"		move.w	#40960,%%d4			\n"
	"		sub.w	%%d2,%%d4			\n"	// d4 = 40960-(512-x)*x/8

	"		divu	%%d4,%%d3			\n"	// d3 = (8192*(512-x)*x) / (40960-(512-x)*x/8)

	"		move.w	%%d3,(%%a0)+		\n"
	"		neg.w	%%d3				\n"
	"		move.w	%%d3,(1022,%%a0)	\n"

	"		addq	#1,%%d0				\n"
	"		subq	#1,%%d1				\n"
	"		bgt.s	.loop%=				\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	Jobbo2Size = size;

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

	// Dan 2
	table = Dan2Table;
	asm volatile(
	"start%=:							\n"
	"		lea		512(%%a0),%%a0		\n"	// a0 = 2nd Quadrant Start
	"		lea		2(%%a0),%%a1		\n"	// a1 = 1st Quadrant End + 2
	"		moveq	#11,%%d0			\n"	// d0.l = x = 11
	"		moveq	#1,%%d1				\n"
	"		ror.w	#2,%%d1				\n"	// d1.l = y = 16384
	"		move.w	#163,%%d2			\n"	// d2 = Q = magic division value = 512/PI (162.97466)
	"		move.w	#256-1,%%d7			\n"	// 256 values in quadrant
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
	"		dbra	%%d7,loop%=			\n"
	"		clr.w	-(%%a1)				\n"	// Set SinTable index 0 to 0
	"		clr.w	(%%a0)				\n"	// Set SinTable index 512 to 0
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	Dan2Size = size;

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
	"		divu.w	#74504/2,%%d2		\n"
	"		lsr.w	#2+1,%%d2			\n"
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
	"    	move.w  #1<<7,%%d3			\n"
	"    	moveq   #0,%%d1				\n"
	".1%=: 	moveq   #97,%%d0			\n"
	"    	ror.l   #7,%%d0				\n"
	"    	move.l  %%d1,%%d2			\n"
	"    	mulu.w  %%d2,%%d2			\n"
	"    	sub.l   %%d2,%%d0			\n"
	"    	swap    %%d0				\n"
	"    	mulu.w  %%d1,%%d0			\n"
	"    	swap    %%d0				\n"
	"    	move.w  %%d0,(%%a0)+		\n"
	"    	neg.w   %%d0				\n"
	"    	move.w  %%d0,-2+1024(%%a0)	\n"
	"    	add.w   %%d3,%%d1			\n"
	"    	beq.b   .e%=				\n"
	"    	bpl.b   .1%=				\n"
	"    	neg.w   %%d3				\n"
	"    	bmi.b   .1%=				\n"
	".e%=:								\n"
	"end%=:								\n"
	"		move.w	#end%=-start%=,%%d7	\n"
	: "+a" (table), "=d" (size) : : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "a1", "a2", "a3", "a4", "a6", "cc", "memory"
	);
	RossSize = size;

	KPrintF("Jobbo (%ld bytes)\n"  , JobboSize ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", JobboTable[i] );
	KPrintF("Jobbo 2 (%ld bytes)\n", Jobbo2Size); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", Jobbo2Table[i]);
	KPrintF("Dan (%ld bytes)\n"    , DanSize   ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", DanTable[i]   );
	KPrintF("Dan 2 (%ld bytes)\n"  , Dan2Size  ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", Dan2Table[i]  );
	KPrintF("A/B 2 (%ld bytes)\n"  , AB2Size   ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", AB2Table[i]   );
	KPrintF("A/B 3 (%ld bytes)\n"  , AB3Size   ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", AB3Table[i]   );
	KPrintF("Ross (%ld bytes)\n"   , RossSize  ); for (int i = 0; i < TABLE_SIZE; i++) KPrintF("%ld\n", RossTable[i]  );
}
