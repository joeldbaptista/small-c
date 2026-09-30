/*
 * Convert s to an unsigned integer in base b.
 * This is a non-standard function.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
atoib(char *s, int b)
{
	int digit, n;

	n = 0;
	while (isspace(*s))
		++s;
	while ((digit = (127 & *s++)) >= '0') {
		if (digit >= 'a')
			digit -= 87;
		else if (digit >= 'A')
			digit -= 55;
		else
			digit -= '0';
		if (digit >= b)
			break;
		n = b * n + digit;
	}
	return n;
}
