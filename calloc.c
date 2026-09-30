/*
 * Allocate n items of size bytes each, cleared to zero.
 * Returns the address of the block, else NULL on failure.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

char *
calloc(unsigned n, unsigned size)
{
	return Ualloc(n * size, YES);
}
