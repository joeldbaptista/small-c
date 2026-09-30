/*
 * Return true if c is alphanumeric.
 */

#include "clib.h"

int
isalnum(int c)
{
	return (c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') ||
	       (c <= '9' && c >= '0');
}
