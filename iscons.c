/*
 * Determine whether fd is the console.
 */

#include "clib.h"

int
iscons(int fd)
{
	return Udevice[fd] == CONSOL;
}
