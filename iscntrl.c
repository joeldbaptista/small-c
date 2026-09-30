/*
 * Return true if c is a control character (0-31 or 127).
 * c is an unsigned quantity, which Small-C faked with a pointer.
 */

#include "clib.h"

int
iscntrl(unsigned c)
{
	return c <= 31 || c == 127;
}
