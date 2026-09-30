/*
 * Rewind a file to its beginning.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
rewind(int fd)
{
	return seek(fd, 0, 0, 0);
}
