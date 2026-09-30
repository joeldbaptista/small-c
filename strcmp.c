/*
 * Return a value < 0, 0 or > 0 as s is < t, = t or > t.
 */

#include "clib.h"

int
strcmp(char *s, char *t)
{
	while (*s == *t) {
		if (*s == 0)
			return 0;
		++s;
		++t;
	}
	return *s - *t;
}
