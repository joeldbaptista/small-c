/*
 * Write a string to standard output.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

void
puts(char *string)
{
	fputs(string, stdout);
	fputc('\n', stdout);
}
