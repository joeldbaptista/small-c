/*
 * Return true if c is an ASCII character (0-127).
 * c is an unsigned quantity, which Small-C faked with a pointer.
 */

#include "clib.h"

int
isascii(unsigned c)
{
	return c <= 127;
}
