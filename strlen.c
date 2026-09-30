/*
 * Return the length of s.
 */

#include "clib.h"

int
strlen(char *s)
{
	char *t;

	t = s - 1;
	while (*++t)
		;
	return t - s;
}
