/*
 * Close a previously opened fd and reopen it.
 *
 * fn   = null-terminated file name.  May be prefixed by a drive letter,
 *        or may be just CON:, RDR:, PUN: or LST:.
 * mode = "a" append, "r" read, "w" write, "u" update.
 * fd   = file descriptor of the pertinent file.
 *
 * Returns the original fd on success, else NULL.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
freopen(char *fn, char *mode, int fd)
{
	if (fclose(fd))
		return NULL;
	if ((fd = Uopen(fn, mode, fd)) == ERR)
		return NULL;
	return fd;
}
