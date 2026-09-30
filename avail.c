/*
 * Return the number of bytes of available memory.
 *
 * On a stack overflow condition, if fatal is non-zero the program
 * aborts with an 'M' clue, otherwise zero is returned.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
avail(int fatal)
{
	char x;

	if (&x < Umemptr) {
		if (fatal)
			exit('M');
		return 0;
	}
	return &x - Umemptr;
}
