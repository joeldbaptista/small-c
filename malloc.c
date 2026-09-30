/*
 * Allocate size bytes.
 * Returns the address of the block, else NULL on failure.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

char *
malloc(unsigned size)
{
	return Ualloc(size, NO);
}
