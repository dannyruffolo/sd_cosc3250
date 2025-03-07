/**
 * @file stdio.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <compiler.h>
#include <stdarg.h>
#include <stddef.h>

extern unsigned int getcpuid(void);

/*
 * Standard in/out/err
 * Note: The C99 specification states that they are macro expansions to a
 * pointer to FILE.  I say, close enough for Xinu.
 * C99 doc: http://www.open-std.org/JTC1/SC22/WG14/www/docs/n1256.pdf
 */

/** @ingroup libxc
 * Standard input  */
//#define stdin ((thrtab[thrcurrent[getcpuid()]]).fdesc[0])
#define stdin	NULL

/** @ingroup libxc
 * Standard output  */
//#define stdout ((thrtab[thrcurrent[getcpuid()]]).fdesc[1])
#define stdout	NULL

/** @ingroup libxc
 * Standard error  */
//#define stderr ((thrtab[thrcurrent[getcpuid()]]).fdesc[2])
#define stderr	NULL

/* Formatted input  */
int _doscan(const char *fmt, va_list ap,
            int (*getch)(long, long), int (*ungetch)(long, long),
            long arg1, long arg2);

int fscanf(int dev, const char *format, ...);

/**
 * @ingroup libxc
 */
#define scanf(fmt, ...)      fscanf(stdin, fmt, __VA_ARGS__)

int sscanf(const char *str, const char *format, ...);

/* Formatted output  */
int _doprnt(const char *format, va_list,
            int (*putc_func)(long, long), long putc_arg);

int fprintf(int dev, const char *format, ...) __printf_format(2, 3);
int printf(const char *format, ...) __printf_format(1, 2);
int sprintf(char *str, const char *format, ...) __printf_format(2, 3);

/* Character and string input and output  */
int fgetc(int dev);
char *fgets(char *s, int n, int dev);
int fputc(long c, long dev);
int fputs(const char *s, int dev);

/** @ingroup libxc */
 #define putchar(c) fputc((c), stdout)

/** @ingroup libxc */
#define getchar() fgetc(stdin)

#endif                          /* _STDIO_H_ */
