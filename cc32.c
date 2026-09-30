/*
 * Small-C compiler -- unary operators, subscripts and calls.
 *
 * Small C, 8088/8086 version -- modified by R. Grehan, BYTE Magazine.
 */

#include <stdio.h>
#include <string.h>

#include "cc.h"

static int hier14(Lvalue *lv);
static int primary(Lvalue *lv);
static void experr(void);
static void callfunction(char *ptr);

int
hier13(Lvalue *lv)
{
	char *ptr;
	int k;

	if (match("++")) { /* ++lval */
		if (hier13(lv) == 0) {
			needlval();
			return 0;
		}
		step(inc, lv);
		return 0;
	} else if (match("--")) { /* --lval */
		if (hier13(lv) == 0) {
			needlval();
			return 0;
		}
		step(dec, lv);
		return 0;
	} else if (match("~")) { /* ~ */
		if (hier13(lv))
			rvalue(lv);
		com();
		lv->val = ~lv->val;
		lv->stg = NULL;
		return 0;
	} else if (match("!")) { /* ! */
		if (hier13(lv))
			rvalue(lv);
		lneg();
		lv->val = !lv->val;
		lv->stg = NULL;
		return 0;
	} else if (match("-")) { /* unary - */
		if (hier13(lv))
			rvalue(lv);
		neg();
		lv->val = -lv->val;
		lv->stg = NULL;
		return 0;
	} else if (match("*")) { /* unary * */
		if (hier13(lv))
			rvalue(lv);
		if ((ptr = lv->sym))
			lv->ind = ptr[TYPE];
		else
			lv->ind = CINT;
		lv->ptr = 0;     /* not a pointer or array */
		lv->isconst = 0; /* not a constant */
		lv->val = 1;     /* omit rvalue() on a call */
		lv->stg = NULL;
		return 1;
	} else if (match("&")) { /* unary & */
		if (hier13(lv) == 0) {
			error("illegal address");
			return 0;
		}
		if ((ptr = lv->sym) == NULL) {
			error("illegal address");
			return 0;
		}
		lv->ptr = ptr[TYPE];
		if (lv->ind)
			return 0;
		/* global and non-array */
		address(ptr);
		lv->ind = ptr[TYPE];
		return 0;
	} else {
		k = hier14(lv);
		if (match("++")) { /* lval++ */
			if (k == 0) {
				needlval();
				return 0;
			}
			step(inc, lv);
			dec(lv->ptr >> 2);
			return 0;
		} else if (match("--")) { /* lval-- */
			if (k == 0) {
				needlval();
				return 0;
			}
			step(dec, lv);
			inc(lv->ptr >> 2);
			return 0;
		}
		return k;
	}
}

static int
hier14(Lvalue *lv)
{
	Lvalue lv2 = {0};
	char *before, *ptr, *start;
	int k;

	k = primary(lv);
	ptr = lv->sym;
	blanks();
	if (cch == '[' || cch == '(') {
		lv->sreg = 1; /* the secondary register will be used */
		for (;;) {
			if (match("[")) { /* [subscript] */
				if (ptr == NULL) {
					error("can't subscript");
					junk();
					needtoken("]");
					return 0;
				} else if (ptr[IDENT] == POINTER) {
					rvalue(lv);
				} else if (ptr[IDENT] != ARRAY) {
					error("can't subscript");
					k = 0;
				}
				setstage(&before, &start);
				lv2.isconst = 0;
				/* lv2 is a deadend on both sides */
				plnge2(NULL, NULL, hier1, &lv2, &lv2);
				needtoken("]");
				if (lv2.isconst) {
					clearstage(before, NULL);
					if (lv2.val) {
						if (ptr[TYPE] == CINT)
							loadconst2(lv2.val
							           << LBPW);
						else
							loadconst2(lv2.val);
						ffadd();
					}
				} else {
					if (ptr[TYPE] == CINT)
						doublereg();
					ffadd();
				}
				lv->ptr = 0;
				lv->ind = ptr[TYPE];
				k = 1;
			} else if (match("(")) { /* function(...) */
				if (ptr == NULL) {
					callfunction(NULL);
				} else if (ptr[IDENT] != FUNCTION) {
					if (k && !lv->val)
						rvalue(lv);
					callfunction(NULL);
				} else {
					callfunction(ptr);
				}
				lv->sym = NULL;
				lv->isconst = 0;
				lv->val = 0;
				k = 0;
			} else {
				return k;
			}
		}
	}
	if (ptr == NULL)
		return k;
	if (ptr[IDENT] == FUNCTION) {
		address(ptr);
		lv->sym = NULL;
		return 0;
	}
	return k;
}

static int
primary(Lvalue *lv)
{
	char sname[NAMESIZE];
	char *ptr;
	int k;

	if (match("(")) { /* (expression,...) */
		do
			k = hier1(lv);
		while (match(","));
		needtoken(")");
		return k;
	}
	memset(lv, 0, sizeof(*lv)); /* clear the lvalue */
	if (symname(sname, YES)) {
		if ((ptr = findloc(sname))) {
			if (ptr[IDENT] == LABEL) {
				experr();
				return 0;
			}
			getloc(ptr);
			lv->sym = ptr;
			lv->ind = ptr[TYPE];
			if (ptr[IDENT] == POINTER) {
				lv->ind = CINT;
				lv->ptr = ptr[TYPE];
			}
			if (ptr[IDENT] == ARRAY) {
				lv->ptr = ptr[TYPE];
				return 0;
			}
			return 1;
		}
		if ((ptr = findglb(sname))) {
			if (ptr[IDENT] != FUNCTION) {
				lv->sym = ptr;
				lv->ind = 0;
				if (ptr[IDENT] != ARRAY) {
					if (ptr[IDENT] == POINTER)
						lv->ptr = ptr[TYPE];
					return 1;
				}
				address(ptr);
				lv->ind = lv->ptr = ptr[TYPE];
				return 0;
			}
		}
		ptr = addsym(sname, FUNCTION, CINT, 0, &glbptr, AUTOEXT);
		lv->sym = ptr;
		lv->ind = 0;
		return 0;
	}
	if (constant(lv) == 0)
		experr();
	return 0;
}

static void
experr(void)
{
	error("invalid expression");
	loadconst(0);
	junk();
}

/* Compile a call.  ptr is the symbol table entry, or NULL. */
static void
callfunction(char *ptr)
{
	int isconst, nargs, val;

	nargs = 0;
	blanks(); /* the open paren was already seen */
	while (streq(lptr, ")") == 0) {
		if (endst())
			break;
		if (ptr) {
			expression(&isconst, &val);
			push();
		} else {
			push();
			expression(&isconst, &val);
			swapstk();
		}
		nargs = nargs + BPW; /* count args * BPW */
		if (match(",") == 0)
			break;
	}
	needtoken(")");
	if (ptr == NULL || streq(ptr + NAME, "CCARGC") == 0)
		loadargc(nargs >> LBPW);
	if (ptr)
		ffcall(ptr + NAME);
	else
		callstk();
	csp = modstk(csp + nargs, YES);
}
