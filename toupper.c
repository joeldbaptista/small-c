/*
 * Return the upper-case of c if it is lower-case, else c.
 */

#include "clib.h"

int
toupper(int c)
{
	if (c <= 'z' && c >= 'a')
		return c - 32;
	return c;
}
