/*
 * Convert an unsigned decimal string to the integer nbr.
 * Returns the field size, else ERR on error.
 */

#include "clib.h"

int
utoi(char *decstr, int *nbr)
{
	int d, t;

	d = 0;
	*nbr = 0;
	while (*decstr >= '0' && *decstr <= '9') {
		t = *nbr;
		t = (10 * t) + (*decstr++ - '0');
		if (t >= 0 && *nbr < 0)
			return ERR;
		d++;
		*nbr = t;
	}
	return d;
}
