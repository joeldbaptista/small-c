/*
 * Left adjust and null terminate a string.
 */

#include "clib.h"

void
left(char *str)
{
	char *str2;

	str2 = str;
	while (*str2 == ' ')
		++str2;
	while ((*str++ = *str2++))
		;
}
