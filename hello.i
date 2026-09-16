#line 1 "/home/peter_cho/lcc/hello.c"
#line 1 "/home/peter_cho/lcc/build-ubuntu24/lcc-build/include/stdio.h"









extern struct _iobuf {
	int	_cnt;
	unsigned char *_ptr;
	unsigned char *_base;
	int	_bufsiz;
	short	_flag;
	char	_file;
} _iob[];







typedef long fpos_t;











typedef unsigned long size_t;




typedef char *__va_list;







extern int remove(const char *);
extern int rename(const char *, const char *);
extern struct _iobuf *tmpfile(void);
extern char *tmpnam(char *);
extern int fclose(struct _iobuf *);
extern int fflush(struct _iobuf *);
extern struct _iobuf *fopen(const char *, const char *);
extern struct _iobuf *freopen(const char *, const char *,struct _iobuf *);
extern void setbuf(struct _iobuf *, char *);
extern int setvbuf(struct _iobuf *, char *, int, size_t);
extern int fprintf(struct _iobuf *, const char *, ...);
extern int fscanf(struct _iobuf *, const char *, ...);
extern int printf(const char *, ...);
extern int scanf(const char *, ...);
extern int sprintf(char *, const char *, ...);
extern int sscanf(const char *, const char *, ...);
extern int vfprintf(struct _iobuf *, const char *, __va_list);
extern int vprintf(const char *, __va_list);
extern int vsprintf(char *, const char *, __va_list);
extern int fgetc(struct _iobuf *);
extern char *fgets(char *, int,struct _iobuf *);
extern int fputc(int,struct _iobuf *);
extern int fputs(const char *,struct _iobuf *);
extern int getc(struct _iobuf *);
extern int getchar(void);
extern char *gets(char *);
extern int putc(int,struct _iobuf *);
extern int putchar(int);
extern int puts(const char *);
extern int ungetc(int,struct _iobuf *);
extern size_t fread(void *, size_t, size_t,struct _iobuf *);
extern size_t fwrite(const void *, size_t, size_t,struct _iobuf *);
extern int fgetpos(struct _iobuf *, fpos_t *);
extern int fseek(struct _iobuf *, long int, int);
extern int fsetpos(struct _iobuf *, const fpos_t *);
extern long int ftell(struct _iobuf *);
extern void rewind(struct _iobuf *);
extern void clearerr(struct _iobuf *);
extern int feof(struct _iobuf *);
extern int ferror(struct _iobuf *);
extern void perror(const char *);













#line 2 "/home/peter_cho/lcc/hello.c"
#line 1 "/home/peter_cho/lcc/build-ubuntu24/lcc-build/include/stdlib.h"









typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;










typedef unsigned char wchar_t;


extern double atof(const char *);
extern int atoi(const char *);
extern long int atol(const char *);
extern double strtod(const char *, char **);
extern long int strtol(const char *, char **, int);
extern unsigned long int strtoul(const char *, char **, int);
extern int rand(void);
extern void srand(unsigned int);
extern void *calloc(size_t, size_t);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern void abort(void);
extern int atexit(void (*)(void));
extern void exit(int);
extern char *getenv(const char *);
extern int system(const char *);
extern void *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
extern void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
extern int abs(int);
extern div_t div(int, int);
extern long int labs(long int);
extern ldiv_t ldiv(long int, long int);
extern int mblen(const char *, size_t);
extern int mbtowc(wchar_t *, const char *, size_t);
extern int wctomb(char *, wchar_t);
extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);


#line 3 "/home/peter_cho/lcc/hello.c"
#line 1 "/home/peter_cho/lcc/build-ubuntu24/lcc-build/include/string.h"











void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
int memcmp(const void *, const void *, size_t);
int strcmp(const char *, const char *);
int strcoll(const char *, const char *);
int strncmp(const char *, const char *, size_t);
size_t strxfrm(char *, const char *, size_t);
void *memchr(const void *, int, size_t);
char *strchr(const char *, int);
size_t strcspn(const char *, const char *);
char *strpbrk(const char *, const char *);
char *strrchr(const char *, int);
size_t strspn(const char *, const char *);
char *strstr(const char *, const char *);
char *strtok(char *, const char *);
void *memset(void *, int, size_t);
char *strerror(int);
size_t strlen(const char *);


#line 4 "/home/peter_cho/lcc/hello.c"

int main(void)
{
    char *message = malloc(32);

    if (message == 0)
        return 1;

    strcpy(message, "Hello, LCC 3.6!");

    printf("%s (length=%zu)\n", message, strlen(message));

    free(message);

    return 0;
}
