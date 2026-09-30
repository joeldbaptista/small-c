/*
 * Convert nbr to an octal string of length sz, right adjusted and
 * blank filled.
 *
 * If sz > 0 the result is terminated with a null byte; if sz = 0 the
 * end of the string is found; if sz < 0 the last byte holds data.
 * Returns str.
 */

#include "clib.h"

char *
itoo(int nbr, char *str, int sz)
{
	int digit;

	if (sz > 0)
		str[--sz] = 0;
	else if (sz < 0)
		sz = -sz;
	else
		while (str[sz] != 0)
			++sz;
	while (sz) {
		digit = nbr & 7;
		nbr = (nbr >> 3) & 8191;
		str[--sz] = digit + 48;
		if (nbr == 0)
			break;
	}
	while (sz)
		str[--sz] = ' ';
	return str;
}
