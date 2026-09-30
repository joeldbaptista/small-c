/*
 * System-level library functions.
 *
 * Modified for MS-DOS 2 by R. Grehan.
 *
 * Every routine here is target code for the MS-DOS 8088 runtime.  The
 * #asm blocks are a Small-C extension, not standard C.
 */

#define NOCCARGC /* no argument count passing */
#define DIR      /* compile the directory option */

#include "clib.h"

/*
 * System variables.
 */
int errno; /* error number holding variable */

int Ucnt = 1; /* argument count for main */
int Uvec[20]; /* argument vectors for main */

/* status of the respective file */
int Ustatus[MAXFILES] = {RDBIT, WRTBIT, RDBIT | WRTBIT};

/* non-disk device assignments */
int Udevice[MAXFILES] = {CONSOL, CONSOL, CONSOL};

/* pigeonhole for ungetc bytes */
int Unextc[MAXFILES] = {EOF, EOF, EOF};

/* map logical file descriptors to physical ones */
int Ufd[MAXFILES] = {0, 1, 2};

char *Umemptr;      /* pointer to free memory */
char Uarg1[] = "*"; /* first argument for main */

/*
 * Process the command line, execute main(), and exit to MS-DOS.
 */
void
Umain(void)
{
	Uparse();
	main(Ucnt, Uvec);
	exit(0);
}

/*
 * Parse the command line and set up argc and argv.
 */
void
Uparse(void)
{
	char *count, *ptr;

	count = 128; /* reserve bytes */
	ptr = Ualloc(count + 1, YES);

	count = Ugcmdtl(ptr); /* get the command tail, null padded */

	Uvec[0] = Uarg1; /* first argument is "*" */
	while (*ptr) {
		if (isspace(*ptr)) {
			++ptr;
			continue;
		}
		switch (*ptr) {
		case '<':
			ptr = Uredirect(ptr, "r", stdin);
			continue;
		case '>':
			if (*(ptr + 1) == '>')
				ptr = Uredirect(ptr + 1, "a", stdout);
			else
				ptr = Uredirect(ptr, "w", stdout);
			continue;
		default:
			if (Ucnt < 20)
				Uvec[Ucnt++] = ptr;
			ptr = Ufield(ptr);
		}
	}
}

int
Ugcmdtl(char *mypt)
{
	/* clang-format off */
#asm
  MOV AH,62H  ;Get program segment prefix
  INT 21H
  MOV AX,DS   ;Get our segment
  MOV ES,AX   ;Will be destination
  MOV DS,BX   ;PSP segment is source
  MOV SI,80H  ;Offset to command tail byte count
  MOV CL,[SI] ;Get byte count
  MOV BX,CX   ;Save for return
  INC SI      ;Bump pointer
  POP AX      ;Return address
  POP DI      ;mypt
  PUSH DI     ;Restore
  PUSH AX
  CLD         ;Set direction
  REP MOVSB   ;Move it in
  MOV BYTE PTR ES:[DI],0 ;Move in Null
  MOV AX,ES   ;Restore our segment
  MOV DS,AX
  XOR CX,CX   ;Zero in CX
#endasm
	/* clang-format on */
}

/*
 * Isolate the next command-line field.
 */
char *
Ufield(char *ptr)
{
	while (*ptr) {
		if (isspace(*ptr)) {
			*ptr = NULL;
			return ++ptr;
		}
		++ptr;
	}
	return ptr;
}

/*
 * Redirect stdin or stdout.
 */
char *
Uredirect(char *ptr, char *mode, int std)
{
	char *fn;

	fn = ++ptr;
	ptr = Ufield(ptr);
	if (Uopen(fn, mode, std) == ERR)
		exit('R');
	return ptr;
}

/*
 * Open a file on the specified file descriptor.
 */
int
Uopen(char *fn, char *mode, int fd)
{
	int pfd;

	if (!strchr("rwau", *mode))
		return ERR;
	Unextc[fd] = EOF;
	if (strcmp(fn, "CON:") == 0) {
		Udevice[fd] = CONSOL;
		Ustatus[fd] = RDBIT | WRTBIT;
		return fd;
	}
	if (strcmp(fn, "LST:") == 0) {
		Udevice[fd] = PRINTR;
		Ustatus[fd] = WRTBIT;
		return fd;
	}
	Udevice[fd] = 0;
	switch (*mode) {
	case 'r':
		if ((pfd = Umsdos(fn, 0, 0, OPNFIL + RACCESS)) == ERR)
			return ERR;
		Ustatus[fd] = RDBIT;
		Ufd[fd] = pfd;
		break;
	case 'u':
		if ((pfd = Umsdos(fn, 0, 0, OPNFIL + RWACCESS)) == ERR)
			return ERR;
		Ustatus[fd] = RDBIT | WRTBIT;
		Ufd[fd] = pfd;
		break;
	case 'w':
		if ((pfd = Umsdos(fn, 0, 0, OPNFIL + WACCESS)) != ERR) {
			Umsdos(0, 0, pfd, CLOFIL);
			Umsdos(fn, 0, 0, DELFIL);
		}
	create:
		if ((pfd = Umsdos(fn, 0, 0, MAKFIL)) == ERR)
			return ERR;
		Ustatus[fd] = EOFBIT | WRTBIT;
		Ufd[fd] = pfd;
		break;
	default: /* append mode */
		if ((pfd = Umsdos(fn, 0, 0, OPNFIL + RWACCESS)) == ERR)
			goto create;
		Ustatus[fd] = RDBIT;
		Ufd[fd] = pfd;
		seek(fd, -1, -1, 2);
		while (fgetc(fd) != EOF)
			;
		Ustatus[fd] = EOFBIT | WRTBIT;
	}
	return fd;
}

/*
 * Binary-stream input from fd.
 */
int
Uread(char *buff, int fd, unsigned n)
{
	unsigned i;
	char ch;

	i = n;
	switch (Umode(fd)) {
	default:
		Useterr(fd);
		return EOF;
	case RDBIT: /* FALLTHROUGH */
	case RDBIT | WRTBIT:;
	}
	if (Unextc[fd] != EOF) {
		*buff++ = Unextc[fd];
		Unextc[fd] = EOF;
		if ((--n) == 0)
			return 1;
	}
	switch (Udevice[fd]) {
	/* PUN and LST cannot occur, since they are write mode */
	case CONSOL:
		while (n--) {
			if ((ch = Uconin()) == FILEOF)
				return EOF;
			*buff++ = ch;
		}
		return i - n;
	default:
		if ((i = Umsdos(buff, n, Ufd[fd], RDFIL)) == ERR)
			return ERR;
		if (i == 0)
			Useteof(fd);
		return i;
	}
}

/*
 * Console character input.
 */
int
Uconin(void)
{
	int ch;

	while (!(ch = Dcio(255)))
		;
	switch (ch) {
	case ABORT:
		exit(0);
	case LF: /* FALLTHROUGH */
	case CR:
		Uconout(LF);
		return Uconout(CR);
	case DEL:
		ch = RUB;
		/* FALLTHROUGH */
	default:
		if (ch < 32) {
			Uconout('^');
			Uconout(ch + 64);
		} else {
			Uconout(ch);
		}
		return ch;
	}
}

/*
 * Special direct keyboard input for MS-DOS.
 */
int
Dcio(int ch)
{
	/* clang-format off */
#asm
  POP SI
  POP DX
  PUSH DX
  PUSH SI
  MOV AH,6   ;Direct I/O
  INT 21H
  JNZ  Dcio1
  XOR AL,AL  ;No char.
Dcio1:
  MOV BL,AL
  XOR BH,BH
#endasm
	/* clang-format on */
}

/*
 * Binary-stream output to fd.
 */
int
Uwrite(char *buff, int fd, unsigned n)
{
	unsigned i;

	i = n;
	switch (Umode(fd)) {
	default:
		Useterr(fd);
		return EOF;
	case WRTBIT: /* FALLTHROUGH */
	case WRTBIT | RDBIT:
	case WRTBIT | EOFBIT:
	case WRTBIT | EOFBIT | RDBIT:;
	}
	switch (Udevice[fd]) {
	/* RDR cannot occur, since it is read mode */
	case CONSOL:
		while (n--)
			Dcio(*buff++);
		return i - n;
	case PRINTR:
		while (n--)
			Umsdos(*buff++, 0, 0, PRTOUT);
		return i - n;
	default:
		return Umsdos(buff, n, Ufd[fd], WRFIL);
	}
}

/*
 * Console character output.
 */
int
Uconout(int ch)
{
	Dcio(ch);
	return ch;
}

/*
 * Return the open mode of fd, else NULL.
 */
int
Umode(unsigned fd)
{
	if (fd < MAXFILES)
		return Ustatus[fd];
	return NULL;
}

/*
 * Set the eof status for fd, disabling future input unless writing is
 * allowed.
 */
void
Useteof(int fd)
{
	Ustatus[fd] |= EOFBIT;
}

/*
 * Clear the eof status for fd.
 */
void
Uclreof(int fd)
{
	Ustatus[fd] &= ~EOFBIT;
}

/*
 * Set the error status for fd.
 */
void
Useterr(int fd)
{
	Ustatus[fd] |= ERRBIT;
}

/*
 * Allocate n bytes of possibly zeroed memory.
 *
 * n     = size of the item in bytes.
 * clear = true if clearing is desired.
 *
 * Returns the address of the allocated block, or NULL if the requested
 * amount of space is not available.
 */
char *
Ualloc(unsigned n, int clear)
{
	char *oldptr;

	if (n < avail(YES)) {
		if (clear)
			pad(Umemptr, NULL, n);
		oldptr = Umemptr;
		Umemptr += n;
		return oldptr;
	}
	return NULL;
}

/*
 * MS-DOS interface.  On an error the code is stored in errno and ERR is
 * returned.
 */
int
Umsdos(int dx, int cx, int bx, int ax)
{
	/* clang-format off */
#asm
  POP SI  ;Return address
  POP AX  ;Load all the registers
  POP BX
  POP CX
  POP DX
  PUSH DX  ;Now restore them
  PUSH CX
  PUSH BX
  PUSH AX
  PUSH SI
  INT 21H  ;Issue the call do DOS
  JNC UMSDOS1  ;Jump if no error
  MOV _ERRNO,AX
  MOV AX,-2    ;ERR
UMSDOS1:
  MOV BX,AX
  XOR CX,CX    ;Zero in CX
#endasm
	/* clang-format on */
}
