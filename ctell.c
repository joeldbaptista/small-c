/*
 * Report the current location in a file.
 *
 * The offset is returned split across offstlo and offsthi, such that
 * the true offset is offstlo + (offsthi << 16).
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

/*
 * Returns ERR on error, EOF for non-file devices, else 0.
 */
int
tell(int fd, int *offstlo, int *offsthi)
{
	if (!Umode(fd) || isatty(fd))
		return EOF;
	return utell(offstlo, offsthi, Ufd[fd]);
}

/*
 * MS-DOS low-level routine behind tell().
 */
int
utell(int *offstlo, int *offsthi, int pfd)
{
	/* clang-format off */
#asm
  POP SI  ;Return address
  POP BX  ;Handle
  PUSH BX ;Restore
  PUSH SI
  XOR DX,DX ;Zero in DX
; Move file pointer 0 from current location
; this will return current location in DX:AX
  MOV AX,4201H
  INT 21H
  JC UTELLC1  ;Jump if error
  MOV BP,SP   ;Get SP
  MOV SI,[BP+6] ;offset lo
  MOV [SI],AX   ;move it in
  MOV SI,[BP+4] ;Offset hi
  MOV [SI],DX   ;Move it in
  XOR AX,AX     ;Return no error
  JMP UTELLC2
 UTELLC1: 
  MOV _ERRNO,AX  ;Return error
  MOV AX,-2
 UTELLC2:
  MOV BX,AX
  XOR CX,CX     ;Zero in CX
  EXTRN _ERRNO:WORD
#endasm
	/* clang-format on */
}
