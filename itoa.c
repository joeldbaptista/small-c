/*
 * Convert n to characters in s.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

void
itoa(int n, char *s)
{
	char *ptr;
	int sign;

	ptr = s;
	if ((sign = n) < 0)            /* record the sign */
		n = -n;                /* make n positive */
	do {                           /* generate digits in reverse order */
		*ptr++ = n % 10 + '0'; /* get the next digit */
	} while ((n = n / 10) > 0); /* delete it */
	if (sign < 0)
		*ptr++ = '-';
	*ptr = '\0';
	reverse(s);
}
