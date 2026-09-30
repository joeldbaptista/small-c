/*
 * Close all open files and exit to MS-DOS.
 * errcode is a character sent to stderr before exiting.
 * Returns to MS-DOS rather than to the caller.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

void
exit(char errcode)
{
	if (errcode)
		Uconout(errcode);
	/*
	 * Closing each handle is not needed under MS-DOS, because
	 * function 4C closes the active handles for us.
	 */
	Umsdos(0, 0, 0, 19456); /* 19456 = 4C00H; 4C in AH terminates */
}

/* clang-format off */
#asm
_abort: JMP    _exit
       PUBLIC  _abort
#endasm
       /* clang-format on */
