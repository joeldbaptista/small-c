/*
 * asm.c -- dropping into 8088 assembly with #asm ... #endasm.
 *
 * The calling convention:
 *   - arguments are pushed left to right, each one 16 bits wide;
 *   - the topmost stack entry on entry is the return address;
 *   - the return value is left in BX;
 *   - CX MUST be zero when you return, because the comparison and
 *     logical emitters build their 0/1 result from it.
 *
 * So with BP pointing at SP, 2[BP] is the last argument and 4[BP] is
 * the one before it.
 */

#include stdio.h

/*
 * Add a and b, returning the result.  This is the example from the
 * original BYTE documentation, with the required XOR CX,CX added.
 */
fadd2(a, b) int a, b;
{
	/* clang-format off */
#asm
    MOV BP,SP           ;get the stack pointer
    MOV AX,2[BP]        ;b
    MOV BX,4[BP]        ;a
    ADD BX,AX           ;result in BX
    XOR CX,CX           ;REQUIRED: leave CX zero
#endasm
	/* clang-format on */
}

/*
 * Read the 8088 flags register into an int.
 */
flags(){
/* clang-format off */
#asm
    PUSHF
    POP BX
    XOR CX,CX
#endasm
    /* clang-format on */
}

main()
{
	printf("fadd2(20, 22) = %d\n", fadd2(20, 22));
	printf("flags = %x\n", flags());
	return 0;
}
