/*
 * Convert a signed decimal string to the integer nbr.
 * Returns the field length, else ERR on error.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
dtoi(char *decstr, int *nbr)
{
	int len, s;

	if (*decstr == '-') {
		s = 1;
		++decstr;
	} else {
		s = 0;
	}
	if ((len = utoi(decstr, nbr)) < 0)
		return ERR;
	if (*nbr < 0)
		return ERR;
	if (s) {
		*nbr = -*nbr;
		return ++len;
	}
	return len;
}
