/*
 * Convert nbr to a signed decimal string of width sz, right adjusted
 * and blank filled.
 *
 * If sz > 0 the result is terminated with a null byte; if sz = 0 the
 * end of the string is found; if sz < 0 the last byte holds data.
 * Returns str.
 */

#include "clib.h"

char *
itod(int nbr, char *str, int sz)
{
	char sgn;

	if (nbr < 0) {
		nbr = -nbr;
		sgn = '-';
	} else {
		sgn = ' ';
	}
	if (sz > 0)
		str[--sz] = NULL;
	else if (sz < 0)
		sz = -sz;
	else
		while (str[sz] != NULL)
			++sz;
	while (sz) {
		str[--sz] = nbr % 10 + '0';
		if ((nbr = nbr / 10) == 0)
			break;
	}
	if (sz)
		str[--sz] = sgn;
	while (sz > 0)
		str[--sz] = ' ';
	return str;
}
