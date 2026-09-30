/*
 * Small-C compiler -- 8088/8086 code generator.
 *
 * Small C, 8088/8086 version -- modified by R. Grehan, BYTE Magazine.
 *
 * BX is the primary register and DX the secondary.  CX must be zero on
 * entry to the comparison and logical emitters, which build their 0/1
 * result with MOV BX,CX followed by a conditional INC BX.
 */

#include <ctype.h>
#include <stdio.h>

#include "cc.h"

static void unpush(char *dest);

/* Print all assembler info before any code is generated. */
void
header(void)
{
	ol(" INCLUDE PROLOG.H");
	beglab = getlabel();
}

/* Print any assembler stuff needed at the end. */
void
trailer(void)
{
	char *ptr;

	cptr = STARTGLB;
	while (cptr < ENDGLB) {
		if (cptr[IDENT] == FUNCTION && cptr[CLASS] == AUTOEXT) {
			external(cptr + NAME);
			ol("NEAR");
		}
		cptr += SYMMAX;
	}
#ifdef UPPER
	if ((ptr = findglb("MAIN")) && ptr[OFFSET] == FUNCTION) {
#else
	if ((ptr = findglb("main")) && ptr[OFFSET] == FUNCTION) {
#endif
		external("Uend"); /* link to the library functions */
		ol("NEAR");
	}
	if (swused) {
		external("CCSWITCH");
		ol("NEAR");
	}
	ol(" INCLUDE EPILOG.H");
	ol(" END");
}

/* Load the argument count before a function call. */
void
loadargc(int val)
{
	if (search("NOCCARGC", macn, NAMESIZE + 2, MACNEND, MACNBR, 0) == 0) {
		if (val) {
			ot(" MOV AL,");
			outdec(val);
			nl();
		} else {
			ol(" XOR AL,AL");
		}
	}
}

/* Declare an entry point. */
void
entry(void)
{
	/* the PUBLIC line is needed for the 8088 -- RG */
	ot(" PUBLIC ");
	outlab(ssname);
	nl();

	outlab(ssname);
	col();
	nl();
}

/* Declare an external reference. */
void
external(const char *name)
{
#ifdef LINK
	ot(" EXTRN ");
	outlab(name); /* ot() is used because MASM needs :class */
	ot(":");
#else
	(void)name;
#endif
}

/* Fetch an object indirect into the primary register. */
void
indirect(Lvalue *lv)
{
	if (lv->ind == CCHAR) {
		ol(" MOV AL,[BX]");
		ol(" CBW");
		ol(" MOV BX,AX");
	} else {
		ol(" MOV BX,[BX]");
	}
}

/* Fetch a static memory cell into the primary register. */
void
getmem(Lvalue *lv)
{
	char *sym;

	sym = lv->sym;
	if (sym[IDENT] != POINTER && sym[TYPE] == CCHAR) {
		ot(" MOV AL,");
		outlab(sym + NAME);
		nl();
		ol(" CBW");
		ol(" MOV BX,AX");
	} else {
		ot(" MOV BX,");
		outlab(sym + NAME);
		nl();
	}
}

/* Fetch the address of the given symbol into the primary register. */
void
getloc(char *sym)
{
	loadconst(getint(sym + OFFSET, OFFSIZE) - csp);
	ol(" ADD BX,SP");
}

/* Store the primary register into a static cell. */
void
putmem(Lvalue *lv)
{
	char *sym;

	ot(" MOV ");
	sym = lv->sym;
	outlab(sym + NAME);
	if (sym[IDENT] != POINTER && sym[TYPE] == CCHAR)
		ot(",BL");
	else
		ot(",BX");
	nl();
}

/* Store the primary register through the address in the secondary. */
void
putstk(Lvalue *lv)
{
	ol(" MOV SI,DX");
	if (lv->ind == CCHAR)
		ol(" MOV [SI],BL");
	else
		ol(" MOV [SI],BX");
}

/* Move the primary register to the secondary. */
void
move(void)
{
	ol(" MOV DX,BX");
}

/* Swap the primary and secondary registers. */
void
swap(void)
{
	ol(" XCHG BX,DX");
}

/* Partial instruction loading an immediate into the primary register. */
void
immed(void)
{
	ot(" MOV BX,");
}

/* Partial instruction loading an immediate into the secondary. */
void
immed2(void)
{
	ot(" MOV DX,");
}

/* Push the primary register onto the stack. */
void
push(void)
{
	ol(" PUSH BX  ;"); /* the trailing "  ;" is padding for unpush() */
	csp = csp - BPW;
}

/* Unpush or pop, as required. */
void
smartpop(Lvalue *lv, char *start)
{
	if (lv->sreg)
		pop(); /* the secondary register was used */
	else
		unpush(start);
}

/*
 * Replace a push already in the staging buffer with a swap, then fix up
 * the stack-relative references that follow it.
 */
static void
unpush(char *dest)
{
	const char *sour;
	char *p;
	int i;

	sour = " XCHG DX,BX";
	while (*sour)
		*dest++ = *sour++;
	p = stagenext;
	while (--p > dest) { /* adjust the stack references */
		if (streq(p, " ADD BX,SP")) {
			--p;
			i = BPW;
			while (isdigit((unsigned char)*(--p))) {
				if ((*p = (char)(*p - i)) < '0') {
					*p = (char)(*p + 10);
					i = 1;
				} else {
					i = 0;
				}
			}
		}
	}
	csp = csp + BPW;
}

/* Pop the stack into the secondary register. */
void
pop(void)
{
	ol(" POP DX");
	csp = csp + BPW;
}

/* Swap the primary register and the top of the stack. */
void
swapstk(void)
{
	ol(" MOV SI,SP");
	ol(" XCHG [SI],BX");
}

/* Process a switch statement. */
void
sw(void)
{
	ffcall("CCSWITCH");
	swused = 1; /* so trailer() emits the EXTRN -- RG */
}

/* Call the named subroutine. */
void
ffcall(const char *sname)
{
	ot(" CALL ");
	outlab(sname); /* changed to outlab() -- RG */
	nl();
}

/* Return from a subroutine. */
void
ffret(void)
{
	ol(" RET");
}

/* Call the address held in the primary register. */
void
callstk(void)
{
	ol(" CALL BX");
}

/* Jump to an internal label number. */
void
jump(int label)
{
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test the primary register and jump if false. */
void
testjump(int label)
{
	ol(" OR BX,BX");
	ol(" JNZ $+5");
	ot(" JMP ");
	printlabel(label);
	nl();
}

/* Test the primary register against zero and jump if false. */
void
zerojump(Jumper oper, int label, Lvalue *lv)
{
	clearstage(lv->stg, NULL); /* purge the conventional code */
	(*oper)(label);
}

/* Define storage according to size. */
void
defstorage(int size)
{
	if (size == 1)
		ot(" DB ");
	else
		ot(" DW ");
}

/* Point to the following object. */
void
point(void)
{
	ol(" DW $+2");
}

/* Modify the stack pointer to the value given. */
int
modstk(int newsp, int save)
{
	int k;

	(void)save;
	k = newsp - csp;
	if (k == 0)
		return newsp;
	if (k > 0) {
		ot(" ADD SP,");
		outdec(k);
	} else {
		ot(" SUB SP,");
		outdec(-k);
	}
	nl();
	return newsp;
}

/* Double the primary register. */
void
doublereg(void)
{
	ol(" ADD BX,BX");
}
