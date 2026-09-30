/*
 * Return true if c is a hexadecimal digit (0-9, A-F or a-f).
 */

#include "clib.h"

int
isxdigit(int c)
{
	return (c <= 'f' && c >= 'a') || (c <= 'F' && c >= 'A') ||
	       (c <= '9' && c >= '0');
}
