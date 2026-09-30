/*
 * Character-stream output of one character to fd.
 * Returns the character written on success, else EOF.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
fputc(int ch, int fd)
{
	char buff;

	switch (ch) {
	case EOF:
		buff = FILEOF;
		break;
	case '\n':
		buff = CR;
		Uwrite(&buff, fd, 1);
		buff = LF;
		break;
	default:
		buff = ch;
	}
	Uwrite(&buff, fd, 1);
	if (Ustatus[fd] & ERRBIT)
		return EOF;
	return ch;
}

/* clang-format off */
#asm
_putc equ   _fputc
     PUBLIC _putc
#endasm
    /* clang-format on */
