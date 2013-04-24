#ifndef __DEF_H__
#define __DEF_H__
#include <future.h>
#include <stdio.h>
#include <stdarg.h>
/* some macro defined in this file */
#define GLIMPSE_OFFSET_OF(type,member) ((size_t)&(((type*)0)->member))
static inline int glimpse_snprintf(char* buffer, size_t size, const char* fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	int ret = vsnprintf(buffer, size, fmt, ap);
	va_end(ap);
	if(ret > size) return size;
	else return ret;
}
#endif
