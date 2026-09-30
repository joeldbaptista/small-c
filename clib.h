/*
 * Small-C runtime library -- shared declarations.
 *
 * This is target code.  It describes the MS-DOS 8088 runtime that
 * Small-C programs link against, not code that runs on the host.  The
 * names below deliberately shadow the host C library: inside this
 * runtime, fopen(), strcpy() and friends are these implementations.
 *
 * Eight files still hold #asm blocks of 8088 assembly, which is a
 * Small-C extension rather than standard C.
 */

#ifndef CLIB_H
#define CLIB_H

#include "clibdef.h"
#include "stdio.h"

/*
 * Argument count intrinsic.  Small-C passes the number of arguments in
 * the AL register; CCARGC() reads it back.  There is no standard C
 * equivalent, so printf() and scanf() walk the stack with it.
 */
int CCARGC(void);

/* the user program's entry point, called by Umain() */
int main(int argc, int *argv);

/* csyslib.c -- system level */
extern int errno;
extern int Ucnt;
extern int Uvec[20];
extern int Ustatus[MAXFILES];
extern int Udevice[MAXFILES];
extern int Unextc[MAXFILES];
extern int Ufd[MAXFILES];
extern char *Umemptr;
extern char Uarg1[];

void Umain(void);
void Uparse(void);
int Ugcmdtl(char *mypt);
char *Ufield(char *ptr);
char *Uredirect(char *ptr, char *mode, int std);
int Uopen(char *fn, char *mode, int fd);
int Uread(char *buff, int fd, unsigned n);
int Uconin(void);
int Dcio(int ch);
int Uwrite(char *buff, int fd, unsigned n);
int Uconout(int ch);
int Umode(unsigned fd);
void Useteof(int fd);
void Uclreof(int fd);
void Useterr(int fd);
char *Ualloc(unsigned n, int clear);
int Umsdos(int dx, int cx, int bx, int ax);

/* file access */
int fopen(char *fn, char *mode);
int freopen(char *fn, char *mode, int fd);
int fclose(int fd);
int fgetc(int fd);
int fputc(int ch, int fd);
char *fgets(char *str, int size, int fd);
char *gets(char *str);
char *Ugets(char *str, int size, int fd, int nl);
int fputs(char *string, int fd);
int fread(char *buf, int sz, int n, int fd);
int read(int fd, char *buf, int n);
int fwrite(char *buf, int sz, int n, int fd);
int write(int fd, char *buf, int n);
int seek(int fd, int offstlo, int offsthi, int base);
int tell(int fd, int *offstlo, int *offsthi);
int utell(int *offstlo, int *offsthi, int pfd);
int rewind(int fd);
int ungetc(int c, int fd);
int feof(int fd);
int ferror(int fd);
void clearerr(int fd);
int unlink(char *fn);
int rename(char *from, char *to);
int Urename(char *from, char *to);
int isatty(int fd);
int iscons(int fd);
int poll(int pause);
void exit(char errcode);

/* formatted transfer */
int fprintf(int argc);
int printf(int argc);
int Uprint(int fd, int *nxtarg);
int fscanf(int argc);
int scanf(int argc);
int Uscan(int fd, int *nxtarg);
void puts(char *string);
int putchar(int ch);
int getchar(void);

/* memory */
int avail(int fatal);
char *malloc(unsigned size);
char *calloc(unsigned n, unsigned size);
char *free(char *ptr);
void pad(char *dest, int ch, unsigned n);

/* command line */
int getarg(int n, char *s, int size, int argc, int *argv);

/* strings */
char *strcat(char *s, char *t);
char *strchr(char *str, char c);
int strcmp(char *s, char *t);
char *strcpy(char *s, char *t);
int strlen(char *s);
char *strncat(char *s, char *t, int n);
int strncmp(char *s, char *t, int n);
char *strncpy(char *dest, char *sour, int n);
char *strrchr(char *s, char c);
int lexcmp(char *s, char *t);
int lexorder(char c1, char c2);
void left(char *str);
void reverse(char *s);

/* character classification */
int isalnum(int c);
int isalpha(int c);
int isascii(unsigned c);
int iscntrl(unsigned c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int toascii(int c);
int tolower(int c);
int toupper(int c);

/* numeric conversion */
int abs(int nbr);
int sign(int nbr);
int atoi(char *s);
int atoib(char *s, int b);
int dtoi(char *decstr, int *nbr);
int otoi(char *octstr, int *nbr);
int utoi(char *decstr, int *nbr);
int xtoi(char *hexstr, int *nbr);
void itoa(int n, char *s);
void itoab(int n, char *s, int b);
char *itod(int nbr, char *str, int sz);
char *itoo(int nbr, char *str, int sz);
char *itou(int nbr, char *str, int sz);
char *itox(int nbr, char *str, int sz);

#endif /* CLIB_H */
