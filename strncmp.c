/*
 * Compare two strings for at most n characters.  Returns a value > 0,
 * = 0 or < 0 as s is > t, = t or < t.
 */

#include "clib.h"

int
strncmp(char *s, char *t, int n)
{
	while (n && *s == *t) {
		if (*s == 0)
			return 0;
		++s;
		++t;
		--n;
	}
	if (n)
		return *s - *t;
	return 0;
}
