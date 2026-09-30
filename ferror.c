/*
 * Test for error status on fd.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
ferror(int fd)
{
	return Ustatus[fd] & ERRBIT;
}
