/*
 * Return a pointer to the first occurrence of c in str, else NULL.
 */

#include "clib.h"

char *
strchr(char *str, char c)
{
	while (*str) {
		if (*str == c)
			return str;
		++str;
	}
	return NULL;
}
