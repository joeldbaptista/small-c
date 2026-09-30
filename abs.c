/*
 * Return the absolute value of nbr.
 */

#include "clib.h"

int
abs(int nbr)
{
	if (nbr < 0)
		return -nbr;
	return nbr;
}
