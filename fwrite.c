/*
 * Item-stream and binary-stream output.
 * Use ferror() to detect errors.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

/*
 * Write n items of sz bytes each from buf to fd.
 * Returns a count of the items written, or zero on error.
 */
int
fwrite(char *buf, int sz, int n, int fd)
{
	if (write(fd, buf, n * sz) == -1)
		return 0;
	return n;
}

/*
 * Write n bytes from buf to fd.
 * Returns a count of the bytes written, or -1 on error.
 */
int
write(int fd, char *buf, int n)
{
	return Uwrite(buf, fd, n);
}
