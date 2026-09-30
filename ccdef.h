/*
 * Small-C compiler -- tunable parameters.
 *
 * Values under "target machine" describe the 8088 object code the
 * compiler emits, not the host the compiler runs on.  They must not be
 * changed to match the host.
 */

#ifndef CCDEF_H
#define CCDEF_H

/* compile options */
#define OPTIMIZE    /* compile the output optimizer */
#define COL         /* terminate labels with a colon */
/* #define UPPER */ /* force symbols to upper case */
#define LINK        /* will be used with a linking loader */

/* target machine */
#define BPW 2     /* bytes per word */
#define LBPW 1    /* log2(BPW) */
#define SBPC 1    /* stack bytes per character */
#define ERRCODE 7 /* exit status after a fatal error */

/* symbol table entry layout, as byte offsets */
#define IDENT 0
#define TYPE 1
#define CLASS 2
#define OFFSET 3
#define NAME 5
#define OFFSIZE (NAME - OFFSET)
#define SYMAVG 10
#define SYMMAX 14

/* symbol table size */
#define NUMLOCS 25
#define STARTLOC symtab
#define ENDLOC (symtab + (NUMLOCS * SYMAVG))
#define NUMGLBS 200
#define STARTGLB ENDLOC
#define ENDGLB (ENDLOC + ((NUMGLBS - 1) * SYMMAX))
#define SYMTBSZ 3050 /* NUMLOCS*SYMAVG + NUMGLBS*SYMMAX */

/* symbol names */
#define NAMESIZE 9
#define NAMEMAX 8

/* "switch" table, measured in ints */
#define SWMAX 60 /* cases per switch */
#define SWSIZ 2  /* ints per case: label, value */
#define SWTABSZ (SWMAX * SWSIZ)

/* "while" queue, measured in ints */
#define WQSIZ 3
#define WQTABSZ 30
#define WQMAX (wq + WQTABSZ - WQSIZ)

/* entry offsets in the while queue */
#define WQSP 0
#define WQLOOP 1
#define WQEXIT 2

/* literal pool */
#define LITABSZ 800
#define LITMAX (LITABSZ - 1)

/* input line */
#define LINEMAX 127
#define LINESIZE 128

/* output staging buffer */
#define STAGESIZE 1200
#define STAGELIMIT (STAGESIZE - 1)

/* macro (define) pool */
#define MACNBR 130
#define MACNSIZE (MACNBR * (NAMESIZE + 2))
#define MACNEND (macn + MACNSIZE)
#define MACQSIZE (MACNBR * 7)
#define MACMAX (MACQSIZE - 1)

/* identifier classes, stored at IDENT */
enum {
	LABEL,
	VARIABLE,
	ARRAY,
	POINTER,
	FUNCTION
};

/*
 * Types, stored at TYPE.  The low two bits make the type unique within
 * a length; the high bits give the length of the object in bytes.
 */
#define CCHAR (1 << 2)
#define CINT (BPW << 2)

/* storage classes, stored at CLASS.  LABEL above doubles as class 0. */
enum {
	STATIC = 1,
	AUTOMATIC,
	EXTERNAL,
	AUTOEXT
};

/* statement types */
enum {
	STIF = 1,
	STWHILE,
	STRETURN,
	STBREAK,
	STCONT,
	STASM,
	STEXPR,
	STDO,
	STFOR,
	STSWITCH,
	STCASE,
	STDEF,
	STGOTO,
	STLABEL
};

#endif /* CCDEF_H */
