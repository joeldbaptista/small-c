/*
 * Clear the error status for fd.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

void
clearerr(int fd)
{
	if (Umode(fd))
		Ustatus[fd] &= ~ERRBIT;
}
