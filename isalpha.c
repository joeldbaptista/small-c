/*
 * Return true if c is alphabetic.
 */

#include "clib.h"

int
isalpha(int c)
{
	return (c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A');
}
