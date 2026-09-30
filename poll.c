/*
 * Poll for console input or interruption.
 */

#define NOCCARGC /* no argument count passing */

#include "clib.h"

int
poll(int pause)
{
	int i;

	i = Dcio(255);
	if (pause) {
		if (i == PAUSE) {
			while (!(i = Dcio(255)))
				;
			if (i == ABORT)
				exit(0);
			return 0;
		}
		if (i == ABORT)
			exit(0);
	}
	return i;
}
