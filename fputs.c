/*
 * Write a null-terminated string to fd.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
fputs(char *string, int fd)
{
	while (*string) {
		if (fputc(*string++, fd) == EOF)
			return EOF;
	}
	return 0;
}
