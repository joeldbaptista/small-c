/*
 * Return true if c is a printable character (32-126).
 */

#include "clib.h"

int
isprint(int c)
{
	return c >= 32 && c <= 126;
}
