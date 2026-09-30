/*
 * Return true if c is a decimal digit.
 */

#include "clib.h"

int
isdigit(int c)
{
	return c <= '9' && c >= '0';
}
