/*
 * Small-C compiler -- lvalues, tests and literals.
 *
 * Small C, 8088/8086 version -- modified by R. Grehan, BYTE Magazine.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "cc.h"

static int number(int *val);
static int pstr(int *val);
static int litchar(void);

/*
 * True when v1 is an int pointer or int array and v2 is neither, so the
 * subscript must be scaled.
 */
int
dbltest(Binop oper, Lvalue *v1, Lvalue *v2)
{
	if (oper != ffadd && oper != ffsub)
		return 0;
	if (v1->ptr != CINT)
		return 0;
	if (v2->ptr)
		return 0;
	return 1;
}

/* Determine the type of a binary operation. */
void
result(Lvalue *lv, Lvalue *lv2)
{
	if (lv->ptr != 0 && lv2->ptr != 0) {
		lv->ptr = 0;
	} else if (lv2->ptr) {
		lv->sym = lv2->sym;
		lv->ind = lv2->ind;
		lv->ptr = lv2->ptr;
	}
}

void
step(Stepop oper, Lvalue *lv)
{
	if (lv->ind) {
		if (lv->sreg) {
			push();
			rvalue(lv);
			(*oper)(lv->ptr >> 2);
			pop();
			store(lv);
			return;
		}
		move();
		lv->sreg = 1;
	}
	rvalue(lv);
	(*oper)(lv->ptr >> 2);
	store(lv);
}

void
store(Lvalue *lv)
{
	if (lv->ind)
		putstk(lv);
	else
		putmem(lv);
}

void
rvalue(Lvalue *lv)
{
	if (lv->sym != NULL && lv->ind == 0)
		getmem(lv);
	else
		indirect(lv);
}

/* Compile a test, branching to label when the expression is false. */
void
test(int label, int parens)
{
	Lvalue lv = {0};
	Binop oper;
	char *before, *start;

	before = start = NULL;
	if (parens)
		needtoken("(");
	for (;;) {
		setstage(&before, &start);
		if (hier1(&lv))
			rvalue(&lv);
		if (match(","))
			clearstage(before, start);
		else
			break;
	}
	if (parens)
		needtoken(")");
	if (lv.isconst) { /* constant expression */
		clearstage(before, NULL);
		if (lv.val)
			return;
		jump(label);
		return;
	}
	if (lv.stg) {           /* stage address of the "oper 0" code */
		oper = lv.oper; /* the operator applied */
		if (oper == ffeq || oper == ule)
			zerojump(eq0, label, &lv);
		else if (oper == ffne || oper == ugt)
			zerojump(ne0, label, &lv);
		else if (oper == ffgt)
			zerojump(gt0, label, &lv);
		else if (oper == ffge)
			zerojump(ge0, label, &lv);
		else if (oper == uge)
			clearstage(lv.stg, NULL);
		else if (oper == fflt)
			zerojump(lt0, label, &lv);
		else if (oper == ult)
			zerojump(ult0, label, &lv);
		else if (oper == ffle)
			zerojump(le0, label, &lv);
		else
			testjump(label);
	} else {
		testjump(label);
	}
	clearstage(before, start);
}

int constexpr(int *val)
{
	char *before, *start;
	int isconst;

	setstage(&before, &start);
	expression(&isconst, val);
	clearstage(before, NULL); /* scratch the generated code */
	if (isconst == 0)
		error("must be constant expression");
	return isconst;
}

/* Load a constant into the primary register. */
void
loadconst(int val)
{
	immed();
	outdec(val);
	nl();
}

/* Load a constant into the secondary register. */
void
loadconst2(int val)
{
	immed2();
	outdec(val);
	nl();
}

int
constant(Lvalue *lv)
{
	lv->isconst = 1; /* assume it will be a constant */
	if (number(&lv->val)) {
		immed();
	} else if (pstr(&lv->val)) {
		immed();
	} else if (qstr(&lv->val)) {
		lv->isconst = 0; /* no: it is a string address */
		immed();
		ot("OFFSET "); /* for MASM -- RG */
		printlabel(litlab);
		outbyte('+');
	} else {
		return 0;
	}
	outdec(lv->val);
	nl();
	return 1;
}

static int
number(int *val)
{
	int k, minus;

	k = minus = 0;
	for (;;) {
		if (match("+"))
			;
		else if (match("-"))
			minus = 1;
		else
			break;
	}
	if (isdigit((unsigned char)cch) == 0)
		return 0;
	while (isdigit((unsigned char)cch))
		k = k * 10 + (inbyte() - '0');
	if (minus)
		k = -k;
	*val = k;
	return 1;
}

void
address(char *ptr)
{
	immed();
	ot("OFFSET ");      /* for MASM -- RG */
	outlab(ptr + NAME); /* append an underscore -- RG */
	nl();
}

/* Parse a character constant, packing up to two characters. */
static int
pstr(int *val)
{
	int k;

	k = 0;
	if (match("'") == 0)
		return 0;
	while (cch != '\'') {
		if (cch == 0)
			break;
		k = (k & 255) * 256 + (litchar() & 255);
	}
	gch();
	*val = k;
	return 1;
}

/* Parse a quoted string into the literal pool. */
int
qstr(int *val)
{
	if (match(quote) == 0)
		return 0;
	*val = litptr;
	while (cch != '"') {
		if (cch == 0)
			break;
		stowlit(litchar(), 1);
	}
	gch();
	litq[litptr++] = 0;
	return 1;
}

void
stowlit(int value, int size)
{
	if ((litptr + size) >= LITMAX) {
		error("literal queue overflow");
		exit(ERRCODE);
	}
	putint(value, litq + litptr, size);
	litptr = litptr + size;
}

/* Return the current literal character and advance lptr. */
static int
litchar(void)
{
	int i, oct;

	if (cch != '\\' || nch == 0)
		return gch();
	gch();
	if (cch == 'n') {
		gch();
		return NEWLINE;
	}
	if (cch == 't') {
		gch();
		return 9; /* HT */
	}
	if (cch == 'b') {
		gch();
		return 8; /* BS */
	}
	if (cch == 'f') {
		gch();
		return 12; /* FF */
	}
	i = 3;
	oct = 0;
	while (i-- > 0 && cch >= '0' && cch <= '7')
		oct = (oct << 3) + gch() - '0';
	if (i == 2)
		return gch();
	return oct;
}
