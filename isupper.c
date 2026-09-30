/*
 * Return true if c is upper-case alphabetic.
 */

#include "clib.h"

int
isupper(int c)
{
	return c <= 'Z' && c >= 'A';
}
