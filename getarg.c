/*
 * Get a command line argument.
 *
 * n    = number of the argument.
 * s    = destination string pointer.
 * size = size of the destination string.
 * argc = argument count from main().
 * argv = argument vector from main().
 *
 * Returns the number of characters moved on success, else EOF.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
getarg(int n, char *s, int size, int argc, int *argv)
{
	char *str;
	int i;

	if (n < 0 || n >= argc) {
		*s = NULL;
		return EOF;
	}
	i = 0;
	str = argv[n];
	while (i < size) {
		if ((s[i] = str[i]) == NULL)
			break;
		++i;
	}
	s[i] = NULL;
	return i;
}
