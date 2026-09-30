/*
 * Formatted read, operating as described by Kernighan & Ritchie.
 * The b, c, d, o, s, u and x specifications are supported; b (binary)
 * is a non-standard extension.
 *
 * As with fprintf(), these functions declare one parameter and walk the
 * stack for the rest using the count Small-C leaves in AL.
 */

#define NOCCARGC
/*
 * Yes, that is correct.  Although these functions use an argument
 * count, they do not call functions which need one.
 */

#include "clib.h"

/*
 * fscanf(fd, ctlstring, arg, arg, ...)
 */
int
fscanf(int argc)
{
	int *nxtarg;

	nxtarg = CCARGC() + &argc;
	return Uscan(*(--nxtarg), --nxtarg);
}

/*
 * scanf(ctlstring, arg, arg, ...)
 */
int
scanf(int argc)
{
	return Uscan(stdin, CCARGC() + &argc - 1);
}

/*
 * Called by fscanf() and scanf().
 */
int
Uscan(int fd, int *nxtarg)
{
	char *carg, *ctl;
	int *narg;
	int ac, base, ch, cnv, ovfl, sign, uval, wast, width;

	ac = 0;
	ctl = *nxtarg--;
	while (*ctl) {
		if (isspace(*ctl)) {
			++ctl;
			continue;
		}
		if (*ctl++ != '%')
			continue;
		if (*ctl == '*') {
			narg = carg = &wast;
			++ctl;
		} else {
			narg = carg = *nxtarg--;
		}
		ctl += utoi(ctl, &width);
		if (!width)
			width = 32767;
		if (!(cnv = *ctl++))
			break;
		while (isspace(ch = fgetc(fd)))
			;
		if (ch == EOF) {
			if (ac)
				break;
			return EOF;
		}
		ungetc(ch, fd);
		switch (cnv) {
		case 'c':
			*carg = fgetc(fd);
			break;
		case 's':
			while (width--) {
				if ((*carg = fgetc(fd)) == EOF)
					break;
				if (isspace(*carg))
					break;
				if (carg != &wast)
					++carg;
			}
			*carg = 0;
			break;
		default:
			switch (cnv) {
			case 'b':
				base = 2;
				sign = 1;
				ovfl = 32767;
				break;
			case 'd':
				base = 10;
				sign = 0;
				ovfl = 3276;
				break;
			case 'o':
				base = 8;
				sign = 1;
				ovfl = 8191;
				break;
			case 'u':
				base = 10;
				sign = 1;
				ovfl = 6553;
				break;
			case 'x':
				base = 16;
				sign = 1;
				ovfl = 4095;
				break;
			default:
				return ac;
			}
			*narg = uval = 0;
			while (width-- && !isspace(ch = fgetc(fd)) &&
			       ch != EOF) {
				if (!sign) {
					if (ch == '-') {
						sign = -1;
						continue;
					}
					sign = 1;
				}
				if (ch < '0')
					return ac;
				if (ch >= 'a')
					ch -= 87;
				else if (ch >= 'A')
					ch -= 55;
				else
					ch -= '0';
				if (ch >= base || uval > ovfl)
					return ac;
				uval = uval * base + ch;
			}
			*narg = sign * uval;
		}
		++ac;
	}
	return ac;
}
