/*
 * Put c back into file fd.
 * Returns c if successful, else EOF.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
ungetc(int c, int fd)
{
	if (!Umode(fd) || Unextc[fd] != EOF || c == EOF)
		return EOF;
	return Unextc[fd] = c;
}
