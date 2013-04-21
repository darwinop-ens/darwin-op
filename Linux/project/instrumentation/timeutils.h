#ifndef __TIMEUTILS_H__
#define __TIMEUTILS_H__

#include <sys/time.h>

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

#endif // __TIMEUTILS_H__

