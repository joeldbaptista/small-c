/*
 * Concatenate at most n bytes from t to the end of s.
 * s must be large enough.
 */

#include "clib.h"

char *
strncat(char *s, char *t, int n)
{
	char *d;

	d = s;
	--s;
	while (*++s)
		;
	while (n--) {
		if ((*s++ = *t++))
			continue;
		return d;
	}
	*s = 0;
	return d;
}
