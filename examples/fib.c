/*
 * fib.c -- Fibonacci, computed twice.
 *
 * Shows: recursion, for loops, and the fact that Small-C ints are
 * 16 bits, so fib(24) is the last value that fits.
 */

#include stdio.h

fib(n) int n;
{
	if (n < 2)
		return n;
	return fib(n - 1) + fib(n - 2);
}

main()
{
	int i;
	int a, b, t;

	for (i = 0; i < 10; ++i)
		printf("fib(%d) = %d\n", i, fib(i));

	a = 0;
	b = 1;
	for (i = 0; i < 10; ++i) {
		t = a + b;
		a = b;
		b = t;
	}
	printf("iterative fib(10) = %d\n", a);
	return 0;
}
