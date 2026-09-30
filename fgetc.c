/*
 * Character-stream input of one character from fd.
 * Returns the next character on success, else EOF.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
fgetc(int fd)
{
	char buff;
	int ch;

	if (Uread(&buff, fd, 1) == EOF) {
		Useteof(fd);
		return EOF;
	}
	ch = buff;
	switch (ch) {
	default:
		return ch;
	case FILEOF:
		Useteof(fd);
		return EOF;
	case CR:
		return '\n';
	case LF: /* NOTE: Uconin() maps LF -> CR */
	    ;
	}
	/*
	 * The original fell off the end here, returning whatever was
	 * left in BX.  EOF makes the LF case deterministic.
	 */
	return EOF;
}

/* clang-format off */
#asm
_getc EQU   _fgetc
     PUBLIC _getc
#endasm
    /* clang-format on */
