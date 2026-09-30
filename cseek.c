/*
 * Position fd to the byte given by "offset" relative to the point given
 * by "base".
 *
 * The offset is split across offstlo, which holds bits 0-15, and
 * offsthi, which holds bits 16-31.  The true offset is therefore
 * offstlo + (offsthi << 16).  The split exists because this version of
 * Small-C has only 16-bit signed integers.
 *
 *   BASE   OFFSET RELATIVE TO
 *     0    first record
 *     1    current record
 *     2    end of file (last record + 1)
 *
 * Returns NULL on success, else EOF.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
seek(int fd, int offstlo, int offsthi, int base)
{
	if (!Umode(fd) || isatty(fd))
		return EOF;
	if (Umsdos(offstlo, offsthi, Ufd[fd], base + POSFIL) == ERR)
		return ERR;
	return NULL;
}
