/*
 * Small-C compiler -- declarations.
 *
 * Small C, 8088/8086 version -- modified by R. Grehan, BYTE Magazine.
 */

#include <stdio.h>

#include "cc.h"

static void declglb(int type, int class);
static void paerror(int j);
static int initials(int size, int ident, int dim);
static void init(int size, int ident, int *dim);
static int needsub(void);
static void doargs(int t);

/* Open an include file. */
void
doinclude(void)
{
	char *cp;

	blanks(); /* skip over to the name */
	if (*lptr == '"' || *lptr == '<') {
		cp = ++lptr;
		while (*cp) {
			if (*cp == '"' || *cp == '>')
				*cp = '\0';
			++cp;
		}
	}
	if ((input2 = fopen(lptr, "r")) == NULL)
		error("open failure on include file");
	/*
	 * Clear the rest of the line so the next read comes from the
	 * new file, if it opened.
	 */
	killline();
}

/* Test for global declarations. */
int
dodeclare(int class)
{
	if (amatch("char", 4)) {
		declglb(CCHAR, class);
		ns();
		return 1;
	} else if (amatch("int", 3) || class == EXTERNAL) {
		declglb(CINT, class);
		ns();
		return 1;
	}
	return 0;
}

/* Declare a static variable. */
static void
declglb(int type, int class)
{
	int j, k;

	for (;;) {
		if (endst())
			return;
		/* both halves must run: "(**p" consumes two stars */
		if (match("(*") | match("*")) {
			j = POINTER;
			k = 0;
		} else {
			j = VARIABLE;
			k = 1;
		}
		if (symname(ssname, YES) == 0)
			illname();
		if (findglb(ssname))
			multidef(ssname);
		if (match(")"))
			;
		if (match("()")) {
			j = FUNCTION;
		} else if (match("[")) {
			paerror(j);
			k = needsub(); /* get the size */
			j = ARRAY;
		}
		if (class == EXTERNAL) { /* changed for MASM */
			external(ssname);
			if (j == FUNCTION) {
				if (k != 0)
					ol("NEAR");
				else
					ol("WORD");
			} else if (type == CCHAR && k != 0) {
				ol("BYTE");
			} else {
				ol("WORD");
			}
		} else if (j != FUNCTION) {
			j = initials(type >> 2, j, k);
		}
		addsym(ssname, j, type, k, &glbptr, class);
		if (match(",") == 0) /* more? */
			return;
	}
}

/* Declare local variables. */
void
declloc(int typ)
{
	int j, k;

	if (swactive)
		error("not allowed in switch");
	if (noloc)
		error("not allowed with goto");
	if (declared < 0)
		error("must declare first in block");
	for (;;) {
		if (endst())
			return;
		if (match("*"))
			j = POINTER;
		else
			j = VARIABLE;
		if (symname(ssname, YES) == 0)
			illname();
		/* no multidef check: block locals are kept together */
		k = BPW;
		if (match("[")) {
			paerror(j);
			if ((k = needsub())) {
				j = ARRAY;
				if (typ == CINT)
					k = k << LBPW;
			} else {
				j = POINTER;
				k = BPW;
			}
		} else if (typ == CCHAR && j == VARIABLE) {
			k = SBPC;
		}
		declared = declared + k;
		addsym(ssname, j, typ, csp - declared, &locptr, AUTOMATIC);
		if (match(",") == 0)
			return;
	}
}

/* Test for a pointer array, which is unsupported. */
static void
paerror(int j)
{
	if (j == POINTER)
		error("no pointer arrays");
}

/* Initialize global objects. */
static int
initials(int size, int ident, int dim)
{
	int savedim;

	litptr = 0;
	if (dim == 0)
		dim = -1;
	savedim = dim;

	/*
	 * This was entry(), but MASM rejects a colon before DB or DW,
	 * so the label is emitted without one.  -- RG
	 */
	ot(" PUBLIC ");
	outlab(ssname);
	nl();
	outlab(ssname);

	if (match("=")) {
		if (match("{")) {
			while (dim) {
				init(size, ident, &dim);
				if (match(",") == 0)
					break;
			}
			needtoken("}");
		} else {
			init(size, ident, &dim);
		}
	}
	if (dim == -1 && dim == savedim) {
		size = BPW;
		stowlit(0, size);
		ident = POINTER;
	}
	dumplits(size);
	dumpzero(size, dim);
	return ident;
}

/* Evaluate one initializer. */
static void
init(int size, int ident, int *dim)
{
	int value;

	if (qstr(&value)) {
		if (ident == VARIABLE || size != 1)
			error("must assign to char pointer or array");
		*dim = *dim - (litptr - value);
		if (ident == POINTER)
			point();
	} else if (constexpr(&value)) {
		if (ident == POINTER)
			error("cannot assign to pointer");
		stowlit(value, size);
		*dim = *dim - 1;
	}
}

/* Get the required array size. */
static int
needsub(void)
{
	int val;

	if (match("]"))
		return 0; /* null size */
	if (constexpr(&val) == 0)
		val = 1;
	if (val < 0) {
		error("negative size illegal");
		val = -val;
	}
	needtoken("]"); /* force a single dimension */
	return val;
}

/*
 * Begin a function.  Called from parse(), this tries to make a function
 * out of the following text.
 */
void
newfunc(void)
{
	char *ptr;

	nogo = 0;            /* enable goto statements */
	noloc = 0;           /* enable block-local declarations */
	lastst = 0;          /* no statement yet */
	litptr = 0;          /* clear the literal pool */
	litlab = getlabel(); /* label the next literal pool */
	locptr = STARTLOC;   /* clear local variables */
	if (monitor)
		lout(line, stderr);
	if (symname(ssname, YES) == 0) {
		error("illegal function or declaration");
		killline(); /* invalidate the line */
		return;
	}
	if (func1) {
		postlabel(beglab);
		func1 = 0;
	}
	if ((ptr = findglb(ssname))) { /* already known? */
		if (ptr[IDENT] != FUNCTION)
			multidef(ssname);
		else if (ptr[OFFSET] == FUNCTION)
			multidef(ssname);
		else {
			/* earlier assumed to be a function */
			ptr[OFFSET] = FUNCTION;
			ptr[CLASS] = STATIC;
		}
	} else {
		addsym(ssname, FUNCTION, CINT, FUNCTION, &glbptr, STATIC);
	}
	if (match("(") == 0)
		error("no open paren");
	entry();
	locptr = STARTLOC;
	argstk = 0;               /* init the argument count */
	while (match(")") == 0) { /* then count the arguments */
		/* any legal name bumps the argument count */
		if (symname(ssname, YES)) {
			if (findloc(ssname))
				multidef(ssname);
			else {
				addsym(ssname, 0, 0, argstk, &locptr,
				       AUTOMATIC);
				argstk = argstk + BPW;
			}
		} else {
			error("illegal argument name");
			junk();
		}
		blanks();
		/* if not a closing paren, it should be a comma */
		if (streq(lptr, ")") == 0) {
			if (match(",") == 0)
				error("no comma");
		}
		if (endst())
			break;
	}
	csp = 0; /* preset the stack pointer */
	argtop = argstk;
	while (argstk) {
		/* now let the user declare those argument types */
		if (amatch("char", 4)) {
			doargs(CCHAR);
			ns();
		} else if (amatch("int", 3)) {
			doargs(CINT);
			ns();
		} else {
			error("wrong number of arguments");
			break;
		}
	}
	statement();
	if (lastst != STRETURN && lastst != STGOTO)
		ffret();
	if (litptr) {
		printlabel(litlab);
		col();
		dumplits(1); /* dump the literals */
	}
}

/*
 * Declare argument types.  Called from newfunc(), this adds an entry to
 * the local symbol table for each named argument.
 */
static void
doargs(int t)
{
	char *argptr;
	int j, legalname;

	for (;;) {
		if (argstk == 0)
			return; /* no arguments */
		if (match("(*") | match("*"))
			j = POINTER;
		else
			j = VARIABLE;
		if ((legalname = symname(ssname, YES)) == 0)
			illname();
		if (match(")"))
			;
		if (match("()"))
			;
		if (match("[")) {
			paerror(j);
			while (inbyte() != ']') { /* skip "[...]" */
				if (endst())
					break;
			}
			j = POINTER; /* add the entry as a pointer */
		}
		if (legalname) {
			if ((argptr = findloc(ssname))) {
				/* add the type and address details */
				argptr[IDENT] = (char)j;
				argptr[TYPE] = (char)t;
				putint(argtop - getint(argptr + OFFSET,
				                       OFFSIZE),
				       argptr + OFFSET, OFFSIZE);
			} else {
				error("not an argument");
			}
		}
		argstk = argstk - BPW; /* count down */
		if (endst())
			return;
		if (match(",") == 0)
			error("no comma");
	}
}
