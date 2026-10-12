#ifndef __ERRNO
#define __ERRNO

#define EDOM	33
#define ERANGE	34
extern int *__errno_location(void);
#define errno (*__errno_location())

#endif /* __ERRNO */
