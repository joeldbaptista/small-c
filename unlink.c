/*
 * Unlink (delete) the named file.
 * fn may be prefixed by a drive letter.
 * Returns NULL on success, else ERR.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
unlink(char *fn)
{
	return Umsdos(fn, 0, 0, DELFIL);
}

/* clang-format off */
#asm
_delete  equ    _unlink
        PUBLIC  _delete
#endasm
    /* clang-format on */
