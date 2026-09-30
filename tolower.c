/*
 * Return the lower-case of c if it is upper-case, else c.
 */

#include "clib.h"

int
tolower(int c)
{
	if (c <= 'Z' && c >= 'A')
		return c + 32;
	return c;
}
