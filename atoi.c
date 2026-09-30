/*
 * Convert s to an integer.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
atoi(char *s)
{
	int n, sign;

	while (isspace(*s))
		++s;
	sign = 1;
	switch (*s) {
	case '-':
		sign = -1;
		/* FALLTHROUGH */
	case '+':
		++s;
	}
	n = 0;
	while (isdigit(*s))
		n = 10 * n + *s++ - '0';
	return sign * n;
}
