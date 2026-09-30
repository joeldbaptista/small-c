/*
 * Free a previously allocated memory block.  Memory must be freed in
 * the reverse order from which it was allocated.
 *
 * ptr is a value returned by calloc() or malloc().
 * Returns ptr on success, else NULL.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

char *
free(char *ptr)
{
	return Umemptr = ptr;
}

/* clang-format off */
#asm
_cfree  equ    _free
       PUBLIC  _cfree
#endasm
    /* clang-format on */
