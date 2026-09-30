/*
 * Convert a hex string to the integer nbr.
 * Returns the field size, else ERR on error.
 */

#include "clib.h"

int
xtoi(char *hexstr, int *nbr)
{
	char *cp;
	int b, d;

	d = *nbr = 0;
	cp = hexstr;
	while (*cp == '0')
		++cp;
	for (;;) {
		switch (*cp) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			b = 48;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			b = 55;
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			b = 87;
			break;
		default:
			return cp - hexstr;
		}
		if (d < 4)
			++d;
		else
			return ERR;
		*nbr = (*nbr << 4) + (*cp++ - b);
	}
}
