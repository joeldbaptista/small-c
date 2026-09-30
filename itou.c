/*
 * Convert nbr to an unsigned decimal string of width sz, right
 * adjusted and blank filled.
 *
 * If sz > 0 the result is terminated with a null byte; if sz = 0 the
 * end of the string is found; if sz < 0 the last byte holds data.
 * Returns str.
 */

#include "clib.h"

char *
itou(int nbr, char *str, int sz)
{
	int lowbit;

	if (sz > 0)
		str[--sz] = NULL;
	else if (sz < 0)
		sz = -sz;
	else
		while (str[sz] != NULL)
			++sz;
	while (sz) {
		lowbit = nbr & 1;
		nbr = (nbr >> 1) & 32767; /* divide by 2 */
		str[--sz] = ((nbr % 5) << 1) + lowbit + '0';
		if ((nbr = nbr / 5) == 0)
			break;
	}
	while (sz)
		str[--sz] = ' ';
	return str;
}
