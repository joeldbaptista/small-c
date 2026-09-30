/*
 * Get the next character from standard input.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
getchar(void)
{
	return fgetc(stdin);
}
