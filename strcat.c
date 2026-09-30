/*
 * Concatenate t to the end of s.  s must be large enough.
 */

#include "clib.h"

char *
strcat(char *s, char *t)
{
	char *d;

	d = s;
	--s;
	while (*++s)
		;
	while ((*s++ = *t++))
		;
	return d;
}
