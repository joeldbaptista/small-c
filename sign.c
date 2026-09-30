/*
 * Return -1, 0 or +1 according to the sign of nbr.
 */

#include "clib.h"

int
sign(int nbr)
{
	if (nbr > 0)
		return 1;
	if (nbr == 0)
		return 0;
	return -1;
}
