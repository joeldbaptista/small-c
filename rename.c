/*
 * Rename a file.
 *
 * from = address of the old filename.
 * to   = address of the new filename.
 *
 * Returns NULL on success, else ERR.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
rename(char *from, char *to)
{
	return Urename(from, to);
}

int
Urename(char *from, char *to)
{
	/* clang-format off */
#asm
  POP SI  ;Return address
  POP DI  ;New name address
  POP DX  ;Current name address
  PUSH DX  ;Restore
  PUSH DI
  PUSH SI
  MOV AX,DS
  MOV ES,AX ;Set extra segment as our own
  MOV AH,56H ;Rename
  INT 21H
  JNC URENAM1
  MOV _ERRNO,AX
  MOV AX,-2
Urenam1:
  MOV BX,AX
  XOR CX,CX  ;Zero in CX
  EXTRN _ERRNO:WORD
#endasm
	/* clang-format on */
}
