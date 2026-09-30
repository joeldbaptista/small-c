/*
 * Formatted print, operating as described by Kernighan & Ritchie.
 * The b, c, d, o, s, u and x specifications are supported; b (binary)
 * is a non-standard extension.
 *
 * These functions take a single declared parameter and then walk the
 * stack for the rest, using the argument count Small-C leaves in AL.
 * That is not standard C and has no stdarg equivalent here, because the
 * calling convention is fixed by the code Small-C generates.
 */

#define NOCCARGC
/*
 * Yes, that is correct.  Although these functions use an argument
 * count, they do not call functions which need one.
 */

#include "clib.h"

/*
 * fprintf(fd, ctlstring, arg, arg, ...)
 */
int
fprintf(int argc)
{
	int *nxtarg;

	nxtarg = CCARGC() + &argc;
	return Uprint(*(--nxtarg), --nxtarg);
}

/*
 * printf(ctlstring, arg, arg, ...)
 */
int
printf(int argc)
{
	return Uprint(stdout, CCARGC() + &argc - 1);
}

/*
 * Called by fprintf() and printf().
 */
int
Uprint(int fd, int *nxtarg)
{
	char *ctl, *sptr, str[17];
	int arg, cc, ladj, len, maxchr, pad, width;

	cc = 0;
	ctl = *nxtarg--;
	while (*ctl) {
		if (*ctl != '%') {
			fputc(*ctl++, fd);
			++cc;
			continue;
		}
		++ctl;
		if (*ctl == '%') {
			fputc(*ctl++, fd);
			++cc;
			continue;
		}
		if (*ctl == '-') {
			ladj = 1;
			++ctl;
		} else {
			ladj = 0;
		}
		if (*ctl == '0')
			pad = '0';
		else
			pad = ' ';
		if (isdigit(*ctl)) {
			width = atoi(ctl++);
			while (isdigit(*ctl))
				++ctl;
		} else {
			width = 0;
		}
		if (*ctl == '.') {
			maxchr = atoi(++ctl);
			while (isdigit(*ctl))
				++ctl;
		} else {
			maxchr = 0;
		}
		arg = *nxtarg--;
		sptr = str;
		switch (*ctl++) {
		case 'c':
			str[0] = arg;
			str[1] = NULL;
			break;
		case 's':
			sptr = arg;
			break;
		case 'd':
			itoa(arg, str);
			break;
		case 'b':
			itoab(arg, str, 2);
			break;
		case 'o':
			itoab(arg, str, 8);
			break;
		case 'u':
			itoab(arg, str, 10);
			break;
		case 'x':
			itoab(arg, str, 16);
			break;
		default:
			return cc;
		}
		len = strlen(sptr);
		if (maxchr && maxchr < len)
			len = maxchr;
		if (width > len)
			width = width - len;
		else
			width = 0;
		if (!ladj) {
			while (width--) {
				fputc(pad, fd);
				++cc;
			}
		}
		while (len--) {
			fputc(*sptr++, fd);
			++cc;
		}
		if (ladj) {
			while (width--) {
				fputc(pad, fd);
				++cc;
			}
		}
	}
	return cc;
}
