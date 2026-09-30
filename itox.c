/*
 * Convert nbr to a hex string of length sz, right adjusted and blank
 * filled.
 *
 * If sz > 0 the result is terminated with a null byte; if sz = 0 the
 * end of the string is found; if sz < 0 the last byte holds data.
 * Returns str.
 */

#include "clib.h"

char *
itox(int nbr, char *str, int sz)
{
	int digit, offset;

	if (sz > 0)
		str[--sz] = 0;
	else if (sz < 0)
		sz = -sz;
	else
		while (str[sz] != 0)
			++sz;
	while (sz) {
		digit = nbr & 15;
		nbr = (nbr >> 4) & 4095;
		if (digit < 10)
			offset = 48;
		else
			offset = 55;
		str[--sz] = digit + offset;
		if (nbr == 0)
			break;
	}
	while (sz)
		str[--sz] = ' ';
	return str;
}
