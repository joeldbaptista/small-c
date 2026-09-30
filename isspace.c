/*
 * Return true if c is a white-space character.
 */

#include "clib.h"

int
isspace(int c)
{
	/* the first test gives a quick exit in most cases */
	return c <= ' ' && (c == ' ' || (c <= 13 && c >= 9));
}
