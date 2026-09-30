# Small-C compiler -- BYTE Small-C 1.0, 8088/8086 target.
#
# The compiler is hosted on a modern system and emits MASM assembly for
# MS-DOS.  The runtime library is target code: it is kept to C99 and is
# style checked, but it is not built here.

CC      = cc
CFLAGS  = -std=c99 -O2 -Wall -Wextra -pedantic

OBJ = cc1.o cc11.o cc12.o cc13.o cc21.o cc22.o cc31.o cc32.o cc33.o \
      cc41.o cc42.o

# Headers that are C.  prolog.h and epilog.h are MASM assembly despite
# the extension, so they are excluded from every C check below.
HDR = cc.h ccdef.h notice.h clib.h clibdef.h stdio.h

# Everything subject to STYLE.md.  The examples are Small-C, so they
# keep K&R function definitions; every other rule still applies, and
# clang-format handles the K&R form correctly.
STYLESRC = $(wildcard *.c) $(HDR) $(wildcard examples/*.c)

all: cc86

cc86: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

$(OBJ): cc.h ccdef.h notice.h

# STYLE.md rule 9 caps lines at 80 columns.  .clang-format sets
# ColumnLimit to 0 and so cannot enforce it, which is why it is checked
# here alongside the formatting itself.
checkstyle:
	@fail=0; \
	for f in $(STYLESRC); do \
		awk -v f="$$f" 'length > 80 { \
			printf "%s:%d: %d columns\n", f, NR, length; n++ \
		} END { exit (n > 0) }' "$$f" || fail=1; \
	done; \
	for f in $(STYLESRC); do \
		clang-format "$$f" | cmp -s "$$f" - || { \
			echo "$$f: not clang-format clean"; fail=1; }; \
	done; \
	if [ $$fail -ne 0 ]; then echo "style: FAILED"; exit 1; fi; \
	echo "style: ok ($(words $(STYLESRC)) files)"

format:
	clang-format -i $(STYLESRC)

clean:
	rm -f cc86 $(OBJ)
	$(MAKE) -C examples clean

.PHONY: all checkstyle format clean
