/*
 * Small-C compiler -- declarations.
 *
 * Small C, 8088/8086 version -- modified by R. Grehan, BYTE Magazine.
 */

#include <stdio.h>
#include <string.h>

#include "cc.h"

static int declglb(int type, int class);
static int dofuncdecl(int class);
static int doparams(void);
static void docparams(void);
static void doknrnames(void);
static void doknrtypes(void);
static void fixargs(void);
static void skipsub(void);
static int istype(void);
static void regfunc(void);
static void startfunc(void);
static void dobody(void);
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
	int type;

	if (amatch("char", 4))
		type = CCHAR;
	else if (amatch("int", 3))
		type = CINT;
	else if (amatch("void", 4))
		type = CINT; /* void has no size; treat it as a word */
	else if (class == EXTERNAL)
		type = CINT;
	else
		return 0;
	/* a function definition brings its own body, and no semicolon */
	if (declglb(type, class) == 0)
		ns();
	return 1;
}

/* Declare a static variable. */
static int
declglb(int type, int class)
{
	int dup, j, k;

	for (;;) {
		if (endst())
			return 0;
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
		/*
		 * Remember a clash but report it later: a prototype may
		 * legally precede the definition of the same function.
		 */
		dup = (findglb(ssname) != NULL);
		if (match(")"))
			;
		if (match("()")) {
			j = FUNCTION;
		} else if (match("(")) {
			/* a parameter list: a prototype or a definition */
			if (dofuncdecl(class))
				return 1;
			j = FUNCTION;
			k = 1;
		} else if (match("[")) {
			paerror(j);
			k = needsub(); /* get the size */
			j = ARRAY;
		}
		if (dup && j != FUNCTION)
			multidef(ssname);
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
			return 0;
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
	char fname[NAMESIZE];

	locptr = STARTLOC; /* clear local variables */
	if (monitor)
		lout(line, stderr);
	if (symname(ssname, YES) == 0) {
		error("illegal function or declaration");
		killline(); /* invalidate the line */
		return;
	}
	startfunc();
	regfunc();
	/* parsing the parameters overwrites ssname, so save the name */
	strcpy(fname, ssname);
	if (match("(") == 0)
		error("no open paren");
	locptr = STARTLOC;
	if (doparams() == 0)
		doknrtypes();
	strcpy(ssname, fname);
	dobody();
}

/*
 * Parse what follows the open paren of a function declarator that was
 * reached through declglb(), meaning a return type was written.
 *
 * Compiles the body and returns 1 when this turns out to be a
 * definition.  Returns 0 for a prototype, leaving the caller to
 * register it.
 */
static int
dofuncdecl(int class)
{
	char fname[NAMESIZE];
	int knr;

	strcpy(fname, ssname);
	locptr = STARTLOC;
	knr = (doparams() == 0);
	if (knr)
		doknrtypes();
	strcpy(ssname, fname);
	blanks();
	if (knr == 0 && streq(lptr, "{") == 0)
		return 0; /* no body follows, so it is a prototype */
	if (class == EXTERNAL)
		error("cannot define an extern function");
	startfunc();
	regfunc();
	dobody();
	return 1;
}

/* Reset the per-function state, and label the program entry once. */
static void
startfunc(void)
{
	nogo = 0;            /* enable goto statements */
	noloc = 0;           /* enable block-local declarations */
	lastst = 0;          /* no statement yet */
	litptr = 0;          /* clear the literal pool */
	litlab = getlabel(); /* label the next literal pool */
	if (func1) {
		postlabel(beglab);
		func1 = 0;
	}
}

/* Record ssname in the global table as a function defined here. */
static void
regfunc(void)
{
	char *ptr;

	if ((ptr = findglb(ssname))) { /* already known? */
		if (ptr[IDENT] != FUNCTION)
			multidef(ssname);
		else if (ptr[OFFSET] == FUNCTION)
			multidef(ssname);
		else {
			/* earlier assumed to be a function, or prototyped */
			ptr[OFFSET] = FUNCTION;
			ptr[CLASS] = STATIC;
		}
	} else {
		addsym(ssname, FUNCTION, CINT, FUNCTION, &glbptr, STATIC);
	}
}

/* Compile a function body.  The parameters are already in scope. */
static void
dobody(void)
{
	entry();
	csp = 0; /* preset the stack pointer */
	statement();
	if (lastst != STRETURN && lastst != STGOTO)
		ffret();
	if (litptr) {
		printlabel(litlab);
		col();
		dumplits(1); /* dump the literals */
	}
}

/* Non-consuming test for a type keyword at the scan position. */
static int
istype(void)
{
	blanks();
	return astreq(lptr, "char", 4) || astreq(lptr, "int", 3) ||
	       astreq(lptr, "void", 4);
}

/* Skip a "[...]" subscript in a parameter declarator. */
static void
skipsub(void)
{
	while (inbyte() != ']') {
		if (endst())
			break;
	}
}

/*
 * Parse a parameter list, the open paren already consumed and the
 * closing one consumed here.
 *
 * Returns 1 for a C99 list, whose types are now recorded, or 0 for a
 * K&R name list whose types still have to be read.
 */
static int
doparams(void)
{
	blanks();
	if (streq(lptr, ")") || istype()) {
		docparams();
		return 1;
	}
	doknrnames();
	return 0;
}

/*
 * Parse a C99 parameter list, in which each parameter carries its own
 * type.  A parameter may be unnamed, as in a prototype.
 */
static void
docparams(void)
{
	int j, t;

	argstk = 0;
	if (match(")")) { /* "()" takes no parameters */
		argtop = 0;
		return;
	}
	for (;;) {
		if (amatch("void", 4)) {
			if (match(")")) /* "(void)" takes none either */
				break;
			t = CINT;
		} else if (amatch("char", 4)) {
			t = CCHAR;
		} else if (amatch("int", 3)) {
			t = CINT;
		} else {
			error("missing parameter type");
			junk();
			break;
		}
		if (match("*"))
			j = POINTER;
		else
			j = VARIABLE;
		if (symname(ssname, YES)) {
			if (match("[")) { /* an array parameter is a pointer */
				paerror(j);
				skipsub();
				j = POINTER;
			}
			if (findloc(ssname))
				multidef(ssname);
			else
				addsym(ssname, j, t, argstk, &locptr,
				       AUTOMATIC);
		} else if (match("[")) {
			skipsub(); /* an unnamed array, as in a prototype */
		}
		argstk = argstk + BPW;
		if (match(",") == 0) {
			needtoken(")");
			break;
		}
	}
	fixargs();
}

/*
 * Parse a K&R parameter list, which names the parameters but gives no
 * types.  Those follow the closing paren and are read by doknrtypes().
 */
static void
doknrnames(void)
{
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
	argtop = argstk;
}

/* Read the type declarations that follow a K&R parameter list. */
static void
doknrtypes(void)
{
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
}

/*
 * Rewrite the parameter offsets once the whole list is known.  Each one
 * was recorded at its position from the left, but the generated code
 * addresses it by its distance from the top of the frame.
 */
static void
fixargs(void)
{
	char *p;

	argtop = argstk;
	p = STARTLOC;
	while (p < locptr) {
		putint(argtop - getint(p + OFFSET, OFFSIZE), p + OFFSET,
		       OFFSIZE);
		p = nextsym(p);
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
