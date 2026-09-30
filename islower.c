/*
 * Return true if c is lower-case alphabetic.
 */

#include "clib.h"

int
islower(int c)
{
	return c <= 'z' && c >= 'a';
}
