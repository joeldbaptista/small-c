/*
 * Write a character to standard output.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
putchar(int ch)
{
	return fputc(ch, stdout);
}
