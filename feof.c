/*
 * Test for end-of-file status.
 * Returns non-zero if fd is at eof, else zero.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
feof(int fd)
{
	return Ustatus[fd] & EOFBIT;
}
