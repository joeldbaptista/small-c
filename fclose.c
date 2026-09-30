/*
 * Close fd.  Returns NULL for success, otherwise ERR.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
fclose(int fd)
{
	if (!Umode(fd))
		return ERR;
	if (!isatty(fd)) {
		if (Umsdos(0, 0, Ufd[fd], CLOFIL) == ERR)
			return ERR;
	}
	return Ustatus[fd] = Udevice[fd] = NULL;
}
