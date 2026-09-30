/*
 * Line input.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

/*
 * Get an entire string, including its newline terminator, or size-1
 * characters, whichever comes first.  The input is terminated by a null
 * character.  Returns str on success, else NULL.
 */
char *
fgets(char *str, int size, int fd)
{
	return Ugets(str, size, fd, 1);
}

/*
 * Get an entire string from stdin, excluding its newline terminator.
 * The user buffer must be large enough to hold the data.
 * Returns str on success, else NULL.
 */
char *
gets(char *str)
{
	return Ugets(str, 32767, stdin, 0);
}

char *
Ugets(char *str, int size, int fd, int nl)
{
	char *next;
	int backup;

	next = str;
	while (--size > 0) {
		switch (*next = fgetc(fd)) {
		case EOF:
			*next = NULL;
			if (next == str)
				return NULL;
			return str;
		case '\n':
			*(next + nl) = NULL;
			return str;
		case RUB:
			if (next > str)
				backup = 1;
			else
				backup = 0;
			goto backout;
		case WIPE:
			backup = next - str;
		backout:
			if (iscons(fd)) {
				fputs("\b \b\b \b", stderr);
				++size;
				while (backup--) {
					fputs("\b \b", stderr);
					if (*--next < 32)
						fputs("\b \b", stderr);
					++size;
				}
				continue;
			}
			/* FALLTHROUGH */
		default:
			++next;
		}
	}
	*next = NULL;
	return str;
}
