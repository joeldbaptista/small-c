/*
 * Copy t to s.
 */

#include "clib.h"

char *
strcpy(char *s, char *t)
{
	char *d;

	d = s;
	while ((*s++ = *t++))
		;
	return d;
}
