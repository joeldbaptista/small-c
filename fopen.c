/*
 * Open the file indicated by fn.
 *
 * fn   = ASCIIZ file name.  May be prefixed by a drive letter, or may
 *        be just CON:, RDR:, PUN: or LST:.
 * mode = "a" append, "r" read, "w" write, "u" update.
 *
 * Returns a file descriptor on success, else NULL.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
fopen(char *fn, char *mode)
{
	int fd;

	fd = 0; /* skip stdin, which is the error return */
	while (++fd < MAXFILES) {
		if (Umode(fd) == NULL) {
			if ((fd = Uopen(fn, mode, fd)) != ERR)
				return fd;
			break;
		}
	}
	return NULL;
}
