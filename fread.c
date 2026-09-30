/*
 * Item-stream and binary-stream input.
 * Use feof() and ferror() to determine the file status.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

/*
 * Read n items of sz bytes each from fd into buf.
 * Returns a count of the items actually read.
 */
int
fread(char *buf, int sz, int n, int fd)
{
	return read(fd, buf, n * sz) / sz;
}

/*
 * Read n bytes from fd into buf.
 * Returns a count of the bytes actually read.
 */
int
read(int fd, char *buf, int n)
{
	return Uread(buf, fd, n);
}
