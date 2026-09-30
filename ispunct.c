/*
 * Return true if c is a punctuation character, meaning anything that is
 * neither a control character nor alphanumeric.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
ispunct(int c)
{
	return !isalnum(c) && !iscntrl(c);
}
