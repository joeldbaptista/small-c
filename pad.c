/*
 * Place n occurrences of ch at dest.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

void
pad(char *dest, int ch, unsigned n)
{
	while (n--)
		*dest++ = ch;
}
