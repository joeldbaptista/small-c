/*
 * Small-C compiler -- 8088/8086 instruction emitters.
 *
 * Small C, 8088/8086 version -- modified by R. Grehan, BYTE Magazine.
 *
 * Every comparison emitter below assumes CX holds zero on entry.
 */

#include <stdio.h>

#include "cc.h"

/* Add the primary and secondary registers, result in the primary. */
void
ffadd(void)
{
	ol(" ADD BX,DX");
}

/* Subtract the primary from the secondary, result in the primary. */
void
ffsub(void)
{
	ol(" SUB DX,BX");
	ol(" MOV BX,DX");
}

/* Multiply the primary and secondary, result in the primary. */
void
ffmult(void)
{
	ol(" MOV AX,DX");
	ol(" IMUL BX");
	ol(" MOV BX,AX");
}

/*
 * Divide the secondary by the primary, leaving the quotient in the
 * primary and the remainder in the secondary.
 */
void
ffdiv(void)
{
	ol(" MOV AX,DX");
	ol(" SUB DX,DX");
	ol(" IDIV BX");
	ol(" MOV BX,AX");
}

/*
 * Remainder of secondary/primary, leaving the remainder in the primary
 * and the quotient in the secondary.
 */
void
ffmod(void)
{
	ffdiv();
	swap();
}

/* Inclusive "or" of the primary and secondary, result in the primary. */
void
ffor(void)
{
	ol(" OR BX,DX");
}

/* Exclusive "or" of the primary and secondary, result in the primary. */
void
ffxor(void)
{
	ol(" XOR BX,DX");
}

/* "and" of the primary and secondary, result in the primary. */
void
ffand(void)
{
	ol(" AND BX,DX");
}

/* Logical negation of the primary register. */
void
lneg(void)
{
	ol(" OR BX,BX");
	ol(" MOV BX,CX");
	ol(" JNZ $+3");
	ol(" INC BX");
}

/*
 * Arithmetic shift right of the secondary register by the number of
 * bits in the primary, result in the primary.
 *
 * 8088 version: BH is not checked for leftover bits.  Might be a
 * problem. -- RG
 */
void
ffasr(void)
{
	ol(" MOV CL,BL");
	ol(" SAR DX,CL");
	ol(" MOV BX,DX");
	ol(" XOR CX,CX");
}

/*
 * Arithmetic shift left of the secondary register by the number of bits
 * in the primary, result in the primary.
 */
void
ffasl(void)
{
	ol(" MOV CL,BL");
	ol(" SAL DX,CL");
	ol(" MOV BX,DX");
	ol(" XOR CX,CX");
}

/* Two's complement of the primary register. */
void
neg(void)
{
	ol(" NEG BX");
}

/* One's complement of the primary register. */
void
com(void)
{
	ol(" NOT BX");
}

/*
 * Increment the primary register by one object of whatever size.
 *
 * 8088 version: altered slightly from the original, since 16-bit maths
 * is easier on the 8088. -- RG
 */
void
inc(int n)
{
	if (n <= 2) {
		do {
			ol(" INC BX");
		} while (--n >= 1);
	} else {
		ot(" ADD BX,");
		outdec(n);
		nl();
	}
}

/* Decrement the primary register by one object of whatever size. */
void
dec(int n)
{
	if (n <= 2) {
		do {
			ol(" DEC BX");
		} while (--n >= 1);
	} else {
		ot(" SUB BX,");
		outdec(n);
		nl();
	}
}

/* Test for equal to. */
void
ffeq(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JNZ $+3");
	ol(" INC BX");
}

/* Test for equal to zero. */
void
eq0(int label)
{
	ol(" OR BX,BX");
	ol(" JZ $+5");
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test for not equal to. */
void
ffne(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JE $+3");
	ol(" INC BX");
}

/* Test for not equal to zero. */
void
ne0(int label)
{
	ol(" OR BX,BX");
	ol(" JNZ $+5");
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test for less than (signed). */
void
fflt(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JGE $+3");
	ol(" INC BX");
}

/* Test for less than zero. */
void
lt0(int label)
{
	ol(" OR BX,BX");
	ol(" JS $+5");
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test for less than or equal to (signed). */
void
ffle(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JG $+3");
	ol(" INC BX");
}

/* Test for less than or equal to zero. */
void
le0(int label)
{
	ol(" OR BX,BX");
	ol(" JLE $+5");
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test for greater than (signed). */
void
ffgt(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JLE $+3");
	ol(" INC BX");
}

/* Test for greater than zero. */
void
gt0(int label)
{
	ol(" OR BX,BX");
	ol(" JG $+5");
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test for greater than or equal to (signed). */
void
ffge(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JL $+3");
	ol(" INC BX");
}

/* Test for greater than or equal to zero. */
void
ge0(int label)
{
	ol(" OR BX,BX");
	ol(" JGE $+5");
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test for less than (unsigned). */
void
ult(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JAE $+3");
	ol(" INC BX");
}

/* Test for less than zero (unsigned), which is never true. */
void
ult0(int label)
{
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test for less than or equal to (unsigned). */
void
ule(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JA $+3");
	ol(" INC BX");
}

/* Test for greater than (unsigned). */
void
ugt(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JBE $+3");
	ol(" INC BX");
}

/* Test for greater than or equal to (unsigned). */
void
uge(void)
{
	ol(" CMP DX,BX");
	ol(" MOV BX,CX");
	ol(" JB $+3");
	ol(" INC BX");
}

/*
 * The peephole optimizer.  It passes the staging buffer through
 * unmodified: the hooks are here, the optimization is not. -- RG
 */
void
peephole(char *ptr)
{
	while (*ptr)
		cout(*ptr++, output);
}
