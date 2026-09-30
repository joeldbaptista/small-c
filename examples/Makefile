# Compile the examples to 8088 MASM assembly.
#
# Build the compiler first, with "make" in the parent directory.
#
# Small-C resolves #include against the current working directory and
# has no search path, so the compiler is run from the parent directory,
# where stdio.h lives.  Compiling an example by hand works the same
# way:
#
#     cd .. && ./cc86 examples/hello.c

TOP  = ..
CC86 = ./cc86
SRC  = $(wildcard *.c)
MAC  = $(SRC:.c=.mac)

all: $(MAC)

%.mac: %.c
	@test -x $(TOP)/$(CC86) || { \
		echo "build the compiler first: make -C .."; exit 1; }
	cd $(TOP) && $(CC86) examples/$< > examples/$@

clean:
	rm -f $(MAC)

.PHONY: all clean
