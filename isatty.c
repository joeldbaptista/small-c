/*
 * Return true if fd is a device, else false.
 */

#include "clib.h"

int
isatty(int fd)
{
	return Udevice[fd];
}
