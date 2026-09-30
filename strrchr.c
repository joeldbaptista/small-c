/*
 * Search s for the rightmost occurrence of c.
 * Returns a pointer to it, else NULL.
 */

#include "clib.h"

char *
strrchr(char *s, char c)
{
	char *ptr;

	ptr = NULL;
	while (*s) {
		if (*s == c)
			ptr = s;
		++s;
	}
	return ptr;
}
